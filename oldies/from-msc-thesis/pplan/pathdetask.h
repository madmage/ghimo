/*
 *  PathDETask.h (Dynamic Probabilistic RoadMaps)
 *
 */

#ifndef __PathDETask_H
#define __PathDETask_H

#include <task.h>
#include <vector>
#include <string>
#include <utility>
#include "ppmap.h"
#include "dptm.h"
#include "elasticstick.h"

class PathDETask: public Task {
private:
	ROrientedPoint* CurrentPosition;
	ROrientedPoint* TargetPosition;
	ROrientedPoint* OldTargetPosition;

	ROrientedPoint* getOrientedPoint(char* InfoName);
	const RPointSet* getPointSet(char* InfoName);
	void refreshPathView(const ppmap::position& CarrotPos);
	void refreshDptmView();
	void refreshMapView();
	void refreshTempObstView();
	void refreshRawBeamsView(const vector<ppmap::position>& Positions);
	void declareSomeEdgeUnsafe(const elasticstick& ElStck);
	bool loadFigMap(char* FileName);

	struct obst_ttl {
		int X, Y;
		clock_t Time;
	};

	struct obst_bmp_cell {
		std::list<obst_ttl>::iterator TTLIterator;
		int Count;
	};

	std::list<obst_ttl> ObstTTLList;
	obst_bmp_cell **ObstBmp;
	void putObstInBmpAndTTLList(const ppmap::position& Position);
	void refreshObstWithTTL();
	void allocObstBmp();
	void freeObstBmp();
	int ObstBmpWidth, ObstBmpHeight;

	std::string RawBeamsDeviceName;
	vector<ppmap::position> getRawBeams();

	unsigned int Iterations;

	// parameters
	bool ParamUseTempObst;
	bool ParamClearObstRays;
	double ParamMaxFunctionDuration;
	double ParamPPMapBmpsQuantization;
	double ParamMaxObstLife;
	double ParamCarrotDistRatio;
	unsigned int ParamObstPersistenceNeed;
	double ParamMinBeamReading, ParamMaxBeamReading;

protected:
	RPointSet* Path;
	RInt* ControllerPolicy;
	ppmap PPMap;
	dptm Dptm;
	elasticstick ElasticStick;
public:
	BakkosNetInfo* PathGeoSetInfo;
	RGeoItemSet PathGeoSet;
	BakkosNetInfo* MapGeoSetInfo;
	RGeoItemSet MapGeoSet;
	BakkosNetInfo* TempObstGeoSetInfo;
	RGeoItemSet TempObstGeoSet;
	BakkosNetInfo* DptmGeoSetInfo;
	RGeoItemSet DptmGeoSet;
	BakkosNetInfo* RawBeamsGeoSetInfo;
	RGeoItemSet RawBeamsGeoSet;

	PathDETask();
	virtual ~PathDETask();

	virtual void process_fn();
	virtual void init();
	virtual void cleanup();

	virtual bool parseCommand(const char* cmd);
};


#endif
