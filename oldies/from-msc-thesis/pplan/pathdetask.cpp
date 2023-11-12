/*
 *  PathDETask.cpp (Dynamic probabilistic topological map + Elastic sticks pathcontrol)
 *
 */

/* TODO

- usare il giusto punto di partenza per i raggi quando si calcola la distanza minima di lettura
  sotto cui non eliminare ostacoli non pi visti
*/

#define TIMES 0

#include <list>
using std::list;
#include <vector>
using std::vector;
#include <utility>
using std::pair;
#include <stdio.h>
#include <sstream>
using std::endl;
#include <ctime>
using std::clock;

#include <netmanager/netinfomanager.h>
#include <rrobotstruct/rorientedpoint.h>

#include <rgeometry/rgeocircle.h>
#include <rgeometry/rgeopointset.h>
#include <rgeometry/rgeoline.h>
#include <rrobotstruct/rint.h>
#include <rstruct/rpointset.h>
#include <netmanager/bakkosnetinfo.h>
#include <netmanager/netinfomanager.h>
#include <netmanager/nettcpsender.h>
#include <task.h>
#include <taskregister.h>
#include <RHI/rangefinder.h>

#include <logging/logging.h>
#define LOGGING_MODULE "PathDETask"

#include "ppmap.h"
#include "dptm.h"
#include "elasticstick.h"
#include "pathdetask.h"

// FIXME perch�segmentation fault se non carica la fig?

PathDETask::PathDETask()
: Task(0, 0, "PathDETask"),
  CurrentPosition(0),
  TargetPosition(0),
  OldTargetPosition(0),
  Path(0),
  PathGeoSetInfo(0),
  ControllerPolicy(0),
  Iterations(0),
  ObstBmp(0)
{
}

PathDETask::~PathDETask() { }

void PathDETask::init()
{
	Path = new RPointSet();
	_reg->setInfoByName("PathPlanner::Trajectory", (const RObject* const*) &Path);

	ControllerPolicy = new RInt();
	ControllerPolicy->val = 1;
	_reg->setInfoByName("PathPlanner::ControllerPolicy", (const RObject* const*) &ControllerPolicy);

	PathGeoSetInfo = new BakkosNetInfo((RObject*) &PathGeoSet, (PeriodicProcess*) getParent(), true);
	_nim->publishInfo(RString("ppTrajectory"), PathGeoSetInfo);

	DptmGeoSetInfo = new BakkosNetInfo((RObject*) &DptmGeoSet, (PeriodicProcess*) getParent(), true);
	_nim->publishInfo(RString("ppDptm"), DptmGeoSetInfo);

	MapGeoSetInfo = new BakkosNetInfo((RObject*) &MapGeoSet, (PeriodicProcess*) getParent(), true);
	_nim->publishInfo(RString("ppMap"), MapGeoSetInfo);

	TempObstGeoSetInfo = new BakkosNetInfo((RObject*) &TempObstGeoSet, (PeriodicProcess*) getParent(), true);
	_nim->publishInfo(RString("ppTempObst"), TempObstGeoSetInfo);

	RawBeamsGeoSetInfo = new BakkosNetInfo((RObject*) &RawBeamsGeoSet, (PeriodicProcess*) getParent(), true);
	_nim->publishInfo(RString("ppRawBeams"), RawBeamsGeoSetInfo);

	Dptm.setPPMap(PPMap);
	ElasticStick.setPPMap(PPMap);

	// FIXME (defaults)
	ParamMaxFunctionDuration = 50.0;
	ParamPPMapBmpsQuantization = 100.0;
	ParamUseTempObst = false;
	ParamMaxObstLife = 0;
	ParamClearObstRays = false;
	ParamObstPersistenceNeed = 0;
	ParamMaxBeamReading = -1;
	ParamMinBeamReading = 0;
	ParamCarrotDistRatio = 1.5;

	if (!m_paramFile){
		RDK_INFO_PRINTF("Parameters file not set, using defaults");
	}
	else {
		ifstream is;
		is.open(m_paramFile);
		if (!is) {
			RDK_INFO_PRINTF("Cannot open parameters file %s, using defaults", m_paramFile);
		}
		else {
			while (is) {
				char s[1024];
				is.getline(s, 1000);
				parseCommand(s);
			}
		}
	}
}

void PathDETask::cleanup()
{
	delete Path;
	delete ControllerPolicy;
	delete PathGeoSetInfo;
	delete MapGeoSetInfo;
	delete DptmGeoSetInfo;
	delete TempObstGeoSetInfo;
	delete RawBeamsGeoSetInfo;
	if (CurrentPosition) delete CurrentPosition;
	if (TargetPosition) delete TargetPosition;
	freeObstBmp();
	_nim->unpublishInfo(RString("ppTrajectory"));
	_nim->unpublishInfo(RString("ppMap"));
	_nim->unpublishInfo(RString("ppDptm"));
	_nim->unpublishInfo(RString("ppTempObst"));
	_nim->unpublishInfo(RString("ppRawBeams"));
}

#define RDK_DEBUG_PRINTF2(message, args...) \
	if (logstep == 0) RDK_DEBUG_PRINTF(message, ## args)

#define RDK_DEBUG_PRINTF1(channel, message, args...) \
	if (logmap[channel] != message) { \
		RDK_DEBUG_PRINTF(message, ## args); \
	} \
	logmap[channel] = message

void PathDETask::process_fn()
{
	if (!_robot) return;
	static map<int, string> logmap;
	static int logstep;
	logstep = (logstep + 1) % 20;

	clock_t tb, ta;			// for local timing
	double fundur;
	clock_t ticksbefore = clock();

	if (CurrentPosition) delete CurrentPosition;
	CurrentPosition = getOrientedPoint("Localization::robotPosition");
	if (!CurrentPosition) {
		double x, y, th;
		_robot->getPosition(x, y, th);
		th = th * DEG2RAD;
		CurrentPosition = new ROrientedPoint();
		CurrentPosition->setX(x);
		CurrentPosition->setY(y);
		CurrentPosition->setTheta(th);
	}

	tb = clock();
	vector<ppmap::position> raw_beams = getRawBeams();
	refreshRawBeamsView(raw_beams);
	ta = clock();
	fundur = (double) (ta - tb) / ((double) CLOCKS_PER_SEC / 1000.0);
	if (TIMES) RDK_DEBUG_PRINTF2("\t%.2f for raw beams", fundur);

	tb = clock();
	if (ParamUseTempObst) {
		for (int i = 0; i < raw_beams.size(); i++) {
			ppmap::position pos(raw_beams[i].X, raw_beams[i].Y);
			if (pos.X < PPMap.getMinX() || pos.X > PPMap.getMaxX() || pos.Y < PPMap.getMinY() || pos.Y > PPMap.getMaxY()) continue;

			if (ParamClearObstRays) PPMap.clearObstRay(ppmap::position(CurrentPosition->getX(), CurrentPosition->getY()), pos);
			putObstInBmpAndTTLList(pos);
		}
	}
	ta = clock();
	fundur = (double) (ta - tb) / ((double) CLOCKS_PER_SEC / 1000.0);
	if (ParamUseTempObst && TIMES) RDK_DEBUG_PRINTF2("\t%.2f for new obstacles", fundur);

	tb = clock();
	refreshObstWithTTL();
	ta = clock();
	fundur = (double) (ta - tb) / ((double) CLOCKS_PER_SEC / 1000.0);
	if (TIMES) RDK_DEBUG_PRINTF2("\t%.2f to refresh obstacles", fundur);

	if (TIMES) RDK_DEBUG_PRINTF2("Current position: %f, %f, %f", CurrentPosition->getX(), CurrentPosition->getY(), CurrentPosition->getTheta());

	tb = clock();
	Dptm.randomPositions(Iterations);
	ta = clock();
	fundur = (double) (ta - tb) / ((double) CLOCKS_PER_SEC / 1000.0);

	RDK_DEBUG_PRINTF2("DPTM Nodes: %d; Edges: %d (%.2f ms each iteration)", Dptm.Nodes.size(), Dptm.Edges.size(), fundur / Iterations);
	if (TIMES) RDK_DEBUG_PRINTF2("\t%.2f for %d dptm iterations", fundur, Iterations);

	dptm::node cur_pos, tar_pos;
	cur_pos.X = CurrentPosition->getX();
	cur_pos.Y = CurrentPosition->getY();

	static bool static_new_topological_path;		// begins with false
	static bool static_force_follow_path;			// begins with false
	static bool static_looking_for_another_topological_path;	// begins with false
	static int static_unsafe_iterations;			// begins with 0
	static es_result static_esres;					// begins with ES_NONE (= 0)
	bool follow_path_now = false;
	bool nothing_to_do = false;
	bool new_target = false;

	if (TargetPosition) delete TargetPosition;
	TargetPosition = getOrientedPoint("Action::GoToPos::Target");

	if (!TargetPosition) {
		RDK_DEBUG_PRINTF2("No GoToPos Target Position: nothing to do");
		static_new_topological_path = false;
		static_unsafe_iterations = 0;
		nothing_to_do = true;
		ElasticStick.Stick.clear();
		static_esres = ES_NONE;
	}
	else {
		tar_pos.X = TargetPosition->getX();
		tar_pos.Y = TargetPosition->getY();
		if (!OldTargetPosition || (OldTargetPosition->getX() != TargetPosition->getX() || OldTargetPosition->getY() != TargetPosition->getY())) {
			RDK_INFO_PRINTF("New target position: %f, %f, %f", TargetPosition->getX(), TargetPosition->getY(), TargetPosition->getTheta() * RAD2DEG);
			new_target = true;
			ElasticStick.Stick.clear();
			static_esres = ES_NONE;
		}
		// FIXME _
		else if (PPMap.posDist(cur_pos, tar_pos) < ElasticStick.ParamMinObstDist / 2) {
			RDK_DEBUG_PRINTF2("Target Position is near to Current one, I've done!");
			nothing_to_do = true;
		}
	}

	es_result esres = ES_NONE;
	elasticstick ES2;
	ES2.setPPMap(PPMap);
	ES2.ParamMinObstDist = ElasticStick.ParamMinObstDist;

	bool using_es2 = false;
	elasticstick* CurES;
	if (static_force_follow_path) {
		CurES = &ES2;
		using_es2 = true;
	}
	else CurES = &ElasticStick;

	if (!nothing_to_do) {
		if (new_target || static_new_topological_path) {
			list<ppmap::position> topological_path = Dptm.findTopologicalPath(cur_pos, tar_pos, !static_looking_for_another_topological_path);
			if (topological_path.size() == 1) {
				RDK_DEBUG_PRINTF1(0, "Unable to find a new topological path (f=%d)", static_force_follow_path);
				esres = ES_NONE;
				static_new_topological_path = true;
				static_looking_for_another_topological_path = false;
				for (dptm::edges_iterator eit = Dptm.Edges.begin(); eit != Dptm.Edges.end(); ++eit) eit->Unsafe = false;
			}
			else {
				RDK_DEBUG_PRINTF1(0, "Topological path found, initializing elastic stick (f=%d)", static_force_follow_path);
				tb = clock();
				CurES->initialize(topological_path);
				esres = CurES->adjust(Iterations);
				ta = clock();
				fundur = (double) (ta - tb) / ((double) CLOCKS_PER_SEC / 1000.0);
				if (TIMES) RDK_DEBUG_PRINTF1(0, "\t%.2f for elastic stick init + adjust (%d iterations)", fundur, Iterations);
				if (static_esres != ES_SAFE && esres == ES_SAFE && CurES == &ES2) {
					RDK_DEBUG_PRINTF1(0, "The other elastic stick is safe, switching elastic sticks");
					ElasticStick = ES2;
				}
				static_esres = esres;
			}
		}

		if (!(new_target || static_new_topological_path) || static_force_follow_path) {
			RDK_DEBUG_PRINTF2("Adjusting elastic stick (es2=%d)", using_es2);
			ElasticStick.Stick.front().X = CurrentPosition->getX();
			ElasticStick.Stick.front().Y = CurrentPosition->getY();
			tb = clock();
			static_esres = ElasticStick.adjust(Iterations);
			ta = clock();
			fundur = (double) (ta - tb) / ((double) CLOCKS_PER_SEC / 1000.0);
			if (TIMES) RDK_DEBUG_PRINTF2("\t%.2f for elastic stick adjust = (%.2f x %d)", fundur, fundur / Iterations, Iterations);
		}

		if (static_esres == ES_SAFE) {
			RDK_DEBUG_PRINTF2("Elastic stick is safe: FOLLOW IT!");
			follow_path_now = true;
			static_new_topological_path = false;
			static_looking_for_another_topological_path = false;
			static_force_follow_path = false;
			static_unsafe_iterations = 0;
			for (dptm::edges_iterator eit = Dptm.Edges.begin(); eit != Dptm.Edges.end(); ++eit) eit->Unsafe = false;
		}
		else {
			if (esres == ES_UNSAFE) {
				RDK_DEBUG_PRINTF1(1, "Elastic stick is unsafe (es2=%d)", using_es2);
			}
			else if (esres == ES_BROKEN) {
				RDK_DEBUG_PRINTF1(1, "Elastic stick is broken (es2=%d)", using_es2);
			}
			else if (esres == ES_NONE) {
				RDK_DEBUG_PRINTF1(1, "Cannot find a path (es2=%d)", using_es2);
				for (dptm::edges_iterator eit = Dptm.Edges.begin(); eit != Dptm.Edges.end(); ++eit) eit->Unsafe = false;
			}

			if (esres == ES_UNSAFE || esres == ES_BROKEN || (static_esres != ES_NONE && esres == ES_NONE)) {
				if (static_unsafe_iterations / 10 % 2 == 0) {
					static_new_topological_path = false;
					follow_path_now = false;
					RDK_DEBUG_PRINTF1(2, "Critical behaviour: waiting (f=%d)", static_force_follow_path);
				}
				else if (static_unsafe_iterations / 10 % 2 == 1) {
					static_new_topological_path = true;
					static_looking_for_another_topological_path = true;
					follow_path_now = false;
					RDK_DEBUG_PRINTF1(2, "Critical behaviour: declaring some edge unsafe (f=%d)", static_force_follow_path);
					declareSomeEdgeUnsafe(ElasticStick);
				}

				if (static_unsafe_iterations > 0 && !static_force_follow_path) {
					static_new_topological_path = false;
					static_force_follow_path = true;
					follow_path_now = true;
					RDK_DEBUG_PRINTF1(2, "Critical behaviour: force follow path");
					for (dptm::edges_iterator eit = Dptm.Edges.begin(); eit != Dptm.Edges.end(); ++eit) {
						eit->Unsafe = false;
					}
				}
				static_unsafe_iterations++;
			}
		}
	}

	if (static_force_follow_path) follow_path_now = true;

	// we do "carrot" stuff
	ppmap::position carrot((double) CurrentPosition->getX(), (double) CurrentPosition->getY());
	if (follow_path_now) {
		double carrotmaxdist = ElasticStick.ParamMinObstDist * ParamCarrotDistRatio;
		double carrotcurdist = 0;
		ppmap::position currobotpos((double) CurrentPosition->getX(), (double) CurrentPosition->getY());
		ppmap::position lastcarrotpos = currobotpos;
		for (list<elasticstick::node>::iterator it = ElasticStick.Stick.begin(); it != ElasticStick.Stick.end(); ++it) {
			if (it->BubbleRadius < ElasticStick.ParamMinObstDist && it != ElasticStick.Stick.begin()) {
				carrot.X = lastcarrotpos.X;
				carrot.Y = lastcarrotpos.Y;
				break;
			}
			double distance = PPMap.posDist(currobotpos, *it);
			if (distance < carrotmaxdist && &*it != &ElasticStick.Stick.back()) {
				carrotcurdist = distance;
				lastcarrotpos = *it;
			}
			else {
				double ratio = (-carrotmaxdist + carrotcurdist) / (carrotcurdist - distance);
				if (ratio < 0) ratio = 0;
				else if (ratio > 1) ratio = 1;
				carrot.X = lastcarrotpos.X + (it->X - lastcarrotpos.X) * ratio;
				carrot.Y = lastcarrotpos.Y + (it->Y - lastcarrotpos.Y) * ratio;
				break;
			}
		}
	}
	Path->reset();
	if (follow_path_now) Path->add((int) carrot.X, (int) carrot.Y);

	if (OldTargetPosition) delete OldTargetPosition;

	if (TargetPosition) {
		OldTargetPosition = new ROrientedPoint();
		*OldTargetPosition = *TargetPosition;
	}

	tb = clock();
	refreshPathView(carrot);
	refreshDptmView();
	refreshTempObstView();
	ta = clock();
	fundur = (double) (ta - tb) / ((double) CLOCKS_PER_SEC / 1000.0);
	if (TIMES) RDK_DEBUG_PRINTF2("\t%.2f to refresh view", fundur);

	clock_t ticksafter = clock();
	fundur = (double) (ticksafter - ticksbefore) / ((double) CLOCKS_PER_SEC / 1000.0);
	RDK_DEBUG_PRINTF2("PathDE Duration: %.2f (max = %.2f) -> Iterations = %d", fundur, ParamMaxFunctionDuration, Iterations);

	if (fundur < ParamMaxFunctionDuration * 0.9) Iterations++;
	else Iterations--;
	if (Iterations < 1) Iterations = 1;
}

/*
	INTERFACE TO RDK
*/

const RPointSet* PathDETask::getPointSet(char* InfoName)
{
	const RObject* const* robj = _reg->getInfoByName(InfoName);
	if (robj) {
		const RPointSet* rpointset = dynamic_cast<const RPointSet*>(*robj);
		if (rpointset) {
			return rpointset;
		}
		else return 0;
	}
	else return 0;
}

ROrientedPoint* PathDETask::getOrientedPoint(char* InfoName)
{
	const RObject* const* robj = _reg->getInfoByName(InfoName);
	if (robj) {
		const ROrientedPoint* rorpoint = dynamic_cast<const ROrientedPoint*>(*robj);
		if (rorpoint) {
			ROrientedPoint* retval = new ROrientedPoint();
			retval->setX(rorpoint->getX());
			retval->setY(rorpoint->getY());
			retval->setTheta(rorpoint->getTheta());
			return retval;
		}
		else return 0;
	}
	else return 0;
}

vector<ppmap::position> PathDETask::getRawBeams()
{
	vector<ppmap::position> retval;
        if (! _robot) return retval;

        RHI::Device* device = _robot->getDevice(RawBeamsDeviceName);
        if (!device) {
		RDK_ERROR_PRINTF("Device '%s' not found.", RawBeamsDeviceName.c_str());
		return retval;
        }

        RHI::RangeFinder* rangefinder = dynamic_cast<RHI::RangeFinder*>(device);
        // XXX gcc 3.2: il test ha successo anche quando non dovrebbe
        if (!rangefinder) {
                RDK_ERROR_PRINTF("Could not downcast Device* to RangeFinder*.");
                return retval;
        }

	RHI::DeviceData* dd = rangefinder->currentData();

	for (int i = 0; i < dd->nrays; i++) {
		double reading = dd->rayData[i]->reading;
		if (reading < ElasticStick.ParamMinObstDist) continue;
		if ((ParamMinBeamReading != -1 && reading < ParamMinBeamReading) || (ParamMaxBeamReading != -1 && reading > ParamMaxBeamReading)) continue;
		double xi = dd->rayInfo[i]->x, yi = dd->rayInfo[i]->y, thetai = dd->rayInfo[i]->theta;
		double xr = CurrentPosition->getX(), yr = CurrentPosition->getY(), thetar = CurrentPosition->getTheta();

		// FIXME
		/* QUESTO FUNZIONA */

		double x = xi * cos(thetar) - yi * sin(thetar);
		double y = xi * sin(thetar) + yi * cos(thetar);
		x = x + cos(thetar + thetai) * reading + xr;
		y = y + sin(thetar + thetai) * reading + yr;
		

		/* DOVREBBE FUNZIONARE PURE QUESTO!! */
		/*
		double x = xi + cos(thetai) * reading;
		double y = yi + sin(thetai) * reading;
		x = x * cos(thetar) - y * sin(thetar) + xr;
		y = x * sin(thetar) + y * cos(thetar) + yr;
		*/

		/* QUESTO E' QUELLO VECCHIO */
		/*
		double x = dd->rayInfo[i]->x + CurrentPosition->getX();
		double y = dd->rayInfo[i]->y + CurrentPosition->getY();
		double theta = dd->rayInfo[i]->theta + CurrentPosition->getTheta();
		x += cos(theta) * reading;
		y += sin(theta) * reading;
		*/
		retval.push_back(ppmap::position(x, y));
	}
}

/*
	REFRESH RCONSOLE VIEW
*/

void PathDETask::refreshPathView(const ppmap::position& CarrotPos)
{
	PathGeoSet.reset();
	for (list<elasticstick::node>::iterator it = ElasticStick.Stick.begin(); it != ElasticStick.Stick.end(); ++it) {
		RGeoCircle* circle = new RGeoCircle(0, it->X, it->Y);
		if (it->BubbleRadius < ElasticStick.ParamMinObstDist) circle->setColor(Red);
		else circle->setColor(Blue);
		PathGeoSet.add(circle);
	}
	RGeoCircle* circle = new RGeoCircle(0, CarrotPos.X, CarrotPos.Y);
	circle->setColor(Cyan);
	PathGeoSet.add(circle);
}

void PathDETask::refreshDptmView()
{
	DptmGeoSet.reset();
	for (dptm::edges_iterator it = Dptm.Edges.begin(); it != Dptm.Edges.end(); ++it) {
		RGeoLine* line = new RGeoLine(0, it->First->X, it->First->Y, it->Second->X, it->Second->Y);
		if (it->Unsafe) line->setColor(Red);
		else line->setColor(Yellow);
		DptmGeoSet.add(line);
	}
	for (dptm::nodes_iterator it = Dptm.Nodes.begin(); it != Dptm.Nodes.end(); ++it) {
		if (it->EdgesPtrs.size() == 0) {
			RGeoCircle* circle = new RGeoCircle(0, it->X, it->Y);
			circle->setColor(Yellow);
			DptmGeoSet.add(circle);
		}
	}
}

void PathDETask::refreshTempObstView()
{
	TempObstGeoSet.reset();
	for (int y = 0; y < PPMap.getObstBmpsHeight(); y++) {
		for (int x = 0; x < PPMap.getObstBmpsWidth(); x++) {
			if (PPMap.TemporaryObstBmp[x][y]) {
				ppmap::position pos(PPMap.bmpToPos(x, y));
				RGeoCircle* circle = new RGeoCircle(0, pos.X, pos.Y);
				circle->setColor(Magenta);
				TempObstGeoSet.add(circle);
			}
		}
	}
}

void PathDETask::refreshMapView()
{
	MapGeoSet.reset();
	for (int i = 0; i < PPMap.StaticObstSegments.size(); i++) {
		RGeoLine* line = new RGeoLine(0, PPMap.StaticObstSegments[i].X1, PPMap.StaticObstSegments[i].Y1,
						 PPMap.StaticObstSegments[i].X2, PPMap.StaticObstSegments[i].Y2);
		line->setColor(Black);
		MapGeoSet.add(line);
	}
}

void PathDETask::refreshRawBeamsView(const vector<ppmap::position>& Positions)
{
	RawBeamsGeoSet.reset();
	for (int i = 0; i < Positions.size(); i++) {
		RGeoPoint* point = new RGeoPoint(0, Positions[i].X, Positions[i].Y);
		point->setColor(White);
		RawBeamsGeoSet.add(point);
	}
}

/*
	UTILITY FUNCTIONS
*/

void PathDETask::declareSomeEdgeUnsafe(const elasticstick& ElStick)
{
	double MAX_DIST = PPMap.getMaxX() + PPMap.getMaxY() - PPMap.getMinX() - PPMap.getMinY();
	double min_distance = MAX_DIST;
	dptm::edge* edge_ptr = 0;
	for (dptm::edges_iterator eit = Dptm.Edges.begin(); eit != Dptm.Edges.end(); ++eit) {
		if (!eit->Unsafe && (PPMap.posSight(*ElStick.CriticalBubblePtr, *eit->First) || PPMap.posSight(*ElStick.CriticalBubblePtr, *eit->Second))) {
			double distance = PPMap.posDistFromSegment(*ElStick.CriticalBubblePtr, *eit->First, *eit->Second);
			if (distance < min_distance) {
				edge_ptr = &*eit;
				min_distance = distance;
			}
		}
	}
	if (edge_ptr) edge_ptr->Unsafe = true;
}

void PathDETask::allocObstBmp()
{
	if (ObstBmp) freeObstBmp();
	ObstBmpWidth = PPMap.getObstBmpsWidth();
	ObstBmpHeight = PPMap.getObstBmpsHeight();
    ObstBmp = new obst_bmp_cell*[ObstBmpWidth];
    for (int i = 0; i < ObstBmpWidth; i++) {
        ObstBmp[i] = new obst_bmp_cell[ObstBmpHeight];
    }
	for (int x = 0; x < ObstBmpWidth; x++) {
		for (int y = 0; y < ObstBmpHeight; y++) {
			ObstBmp[x][y].TTLIterator = ObstTTLList.end();
			ObstBmp[x][y].Count = 0;
		}
	}
}

void PathDETask::freeObstBmp()
{
	if (!ObstBmp) return;
	for (int i = 0; i < ObstBmpWidth; i++) delete[] ObstBmp[i];
	delete[] ObstBmp;
}

void PathDETask::putObstInBmpAndTTLList(const ppmap::position& Pos)
{
	int x = PPMap.posToBmpX(Pos);
	int y = PPMap.posToBmpY(Pos);
	if (x < 0 || y < 0 || x > PPMap.getObstBmpsWidth() - 1 || y > PPMap.getObstBmpsHeight() - 1) return;

	obst_ttl ot;
	ot.X = x;
	ot.Y = y;
	ot.Time = clock();

	int ct = 0;
	if (ObstBmp[x][y].TTLIterator != ObstTTLList.end()) {
		// we cannot just refresh the obstacle, because of the order in the ttl list
		// so we remove it and then re-add
		ct = ObstBmp[x][y].Count + 1;
		ObstTTLList.erase(ObstBmp[x][y].TTLIterator);
	}
	ObstTTLList.push_back(ot);
	list<obst_ttl>::iterator it = ObstTTLList.end();
	--it;	// iterator to the last item

	ObstBmp[x][y].TTLIterator = it;
	ObstBmp[x][y].Count = ct;
	if (ct >= ParamObstPersistenceNeed || PPMap.posDist(ppmap::position(CurrentPosition->getX(), CurrentPosition->getY()), Pos) < ParamMinBeamReading) PPMap.TemporaryObstBmp[x][y] = true;
	//FIXME ^
}

void PathDETask::refreshObstWithTTL()
{
	if (ParamMaxObstLife == -1) return;
	clock_t d = clock() - (clock_t) ((double) ParamMaxObstLife / 1000 * CLOCKS_PER_SEC);
	for (list<obst_ttl>::iterator it = ObstTTLList.begin(); it != ObstTTLList.end(); ) {
		ppmap::position p = PPMap.bmpToPos(it->X, it->Y);
		if (it->Time < d && PPMap.posDist(p, ppmap::position(CurrentPosition->getX(), CurrentPosition->getY())) >= ParamMinBeamReading) {
			// FIXME ^
			PPMap.TemporaryObstBmp[it->X][it->Y] = false;
			ObstBmp[it->X][it->Y].TTLIterator = ObstTTLList.end();
			ObstBmp[it->X][it->Y].Count = 0;
			it = ObstTTLList.erase(it);
		}
		else ++it;		// FIXME the ttl list is ordered by ttl time
	}
}

bool PathDETask::loadFigMap(char* FileName)
{
	simple_fig_loader figloader;
	if (!figloader.loadFig(FileName)) {
		return false;
	}

	int minx = 0, miny = 0, maxx = 0, maxy = 0;
	for (unsigned int i = 0; i < figloader.Segments.size(); i++) {
		if (figloader.Segments[i].X1 < minx) minx = (int) figloader.Segments[i].X1;
		if (figloader.Segments[i].X2 < minx) minx = (int) figloader.Segments[i].X2;
		if (figloader.Segments[i].Y1 < miny) miny = (int) figloader.Segments[i].Y1;
		if (figloader.Segments[i].Y2 < miny) miny = (int) figloader.Segments[i].Y2;
		if (figloader.Segments[i].X1 > maxx) maxx = (int) figloader.Segments[i].X1;
		if (figloader.Segments[i].X2 > maxx) maxx = (int) figloader.Segments[i].X2;
		if (figloader.Segments[i].Y1 > maxy) maxy = (int) figloader.Segments[i].Y1;
		if (figloader.Segments[i].Y2 > maxy) maxy = (int) figloader.Segments[i].Y2;
	}

	PPMap.StaticObstSegments = figloader.Segments;
	PPMap.setLimits(minx, miny, maxx, maxy);
	PPMap.allocObstBmps(ParamPPMapBmpsQuantization);
	allocObstBmp();
	PPMap.UseObstBmps = true;
	PPMap.UseObstSegments = true;
	return true;
}

/*
	PARSE COMMAND
*/

#define CMD_IS(cmd) (strncmp(cmd, c, strlen(cmd)) == 0)
bool PathDETask::parseCommand(const char* c)
{
	if (c[0] == '#') return true;	// just a comment
	else if (CMD_IS("ppMaxFuncDuration")) {
		float v;
		sscanf(c, "%*s %f", &v);
		ParamMaxFunctionDuration = v;
		RDK_INFO_PRINTF("Set MaxFunctionDuration to %.2f ms", ParamMaxFunctionDuration);
		return true;
	}
	else if (CMD_IS("ppBmpsQuantization")) {
		float v;
		sscanf(c, "%*s %f", &v);
		ParamPPMapBmpsQuantization = v;
		RDK_INFO_PRINTF("Set PPMap/BmpsQuantization to %.2f (clear obst bmp)", ParamPPMapBmpsQuantization);
		PPMap.allocObstBmps(ParamPPMapBmpsQuantization);
		allocObstBmp();
		return true;
	}
	else if (CMD_IS("ppSetDeviceName")) {
		char buffer[1024];
		sscanf(c, "%*s %s", buffer);
		RawBeamsDeviceName = (string) buffer;
		RDK_INFO_PRINTF("Set DeviceName for RawBeams reading to '%s'", buffer);
		return true;
	}
	else if (CMD_IS("ppLoadFigMap")) {
		char buffer[1024];
		sscanf(c, "%*s %s", buffer);
		if (loadFigMap(buffer)) {
			RDK_INFO_PRINTF("FigMap (%s) loaded (%d segments)", buffer, PPMap.StaticObstSegments.size());
			refreshMapView();
		}
		else {
			RDK_ERROR_PRINTF("Cannot load FigMap (%s)", buffer);
		}
		return true;
	}
	else if (CMD_IS("ppResetObst")) {
		for (int x = 0; x < PPMap.getObstBmpsWidth(); x++) {
			for (int y = 0; y < PPMap.getObstBmpsHeight(); y++) {
				PPMap.TemporaryObstBmp[x][y] = false;
			}
		}
		RDK_INFO_PRINTF("Reset Temporary Obst Bitmap");
	}
	else if (CMD_IS("ppMaxObstLife")) {
		float f;
		sscanf(c, "%*s %f", &f);
		ParamMaxObstLife = f;
		RDK_INFO_PRINTF("Set MaxObstLife to %.2f ms", ParamMaxObstLife);
	}
	else if (CMD_IS("ppObstPersNeed")) {
		sscanf(c, "%*s %d", &ParamObstPersistenceNeed);
		RDK_INFO_PRINTF("Set ObstPers(istence)Need to %d iterations", ParamObstPersistenceNeed);
	}
	else if (CMD_IS("ppResetDptm")) {
		Dptm.Edges.clear();
		Dptm.Nodes.clear();
		RDK_INFO_PRINTF("Dptm reset");
	}
	else if (CMD_IS("ppResetAll")) {
		parseCommand("ppResetObst");
		parseCommand("ppResetDptm");
	}
	else if (CMD_IS("ppUseTempObst")) {
		int a;
		sscanf(c, "%*s %d", &a);
		ParamUseTempObst = (a > 0);
		RDK_INFO_PRINTF("UseTempObst: %d", a);
		if (!a) parseCommand("ppResetObst");
		return true;
	}
	else if (CMD_IS("ppMaxBeamReading")) {
		float v;
		sscanf(c, "%*s %f", &v);
		ParamMaxBeamReading = v;
		RDK_INFO_PRINTF("Set MaxBeamReading to %.2f mm", ParamMaxBeamReading);
		return true;
	}
	else if (CMD_IS("ppMinBeamReading")) {
		float v;
		sscanf(c, "%*s %f", &v);
		ParamMinBeamReading = v;
		RDK_INFO_PRINTF("Set MinBeamReading to %.2f mm", ParamMinBeamReading);
		return true;
	}
	else if (CMD_IS("ppCarrotDistRatio")) {
		float v;
		sscanf(c, "%*s %f", &v);
		ParamCarrotDistRatio = v;
		RDK_INFO_PRINTF("Set CarrotDistRatio to %.2f mm", ParamCarrotDistRatio);
		return true;
	}
	else if (CMD_IS("ppClearObstRays")) {
		int a;
		sscanf(c, "%*s %d", &a);
		ParamClearObstRays = (a > 0);
		RDK_INFO_PRINTF("ClearObstRays: %d", a);
		return true;
	}
	else if (CMD_IS("ppDptmLFWinner")) {
		float v;
		sscanf(c, "%*s %f", &v);
		Dptm.ParamLearnFactorWinner = v;
		RDK_INFO_PRINTF("Set DPTM/LearnFactorWinner to %.2f", Dptm.ParamLearnFactorWinner);
		return true;
	}
	else if (CMD_IS("ppDptmLFSecond")) {
		float v;
		sscanf(c, "%*s %f", &v);
		Dptm.ParamLearnFactorSecondWinner = v;
		RDK_INFO_PRINTF("Set DPTM/LearnFactorSecondWinner to %.2f", Dptm.ParamLearnFactorSecondWinner);
		return true;
	}
	else if (CMD_IS("ppDptmMaxEdgeAge")) {
		sscanf(c, "%*s %d", &Dptm.ParamMaxEdgeAge);
		RDK_INFO_PRINTF("Set DPTM/MaxEdgeAge to %d iterations", Dptm.ParamMaxEdgeAge);
		return true;
	}
	else if (CMD_IS("ppDptmMaxRandomTries")) {
		sscanf(c, "%*s %d", &Dptm.ParamMaxRandomTries);
		RDK_INFO_PRINTF("Set DPTM/MaxRandomTries to %d", Dptm.ParamMaxRandomTries);
		return true;
	}
	else if (CMD_IS("ppEsMinObstDist")) {
		float v;
		sscanf(c, "%*s %f", &v);
		ElasticStick.ParamMinObstDist = v;
		RDK_INFO_PRINTF("Set ES/MinObstDistance to %.2f mm", ElasticStick.ParamMinObstDist);
		return true;
	}
	else return false;
}

TASK_FACTORY(PathDETask);
