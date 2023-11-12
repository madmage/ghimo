#include <list>
using std::list;
#include <fstream>      // FIXME
using std::ofstream;
using std::endl;


#include "ppmap.h"
#include "elasticstick.h"

ofstream e_os; // FIXME

// TODO: se le bolle sono più di 100 (?) considera meno punti della topological path

elasticstick::elasticstick() :
    ParamMinObstDist(500),
    ParamMinSteeringRadius(0),
    CriticalBubblePtr(0)
{
#ifdef __BORLANDC__
    e_os.open("es-log.txt"); // FIXME
#else
    e_os.open("pathde/logs/es-log");
#endif
}

elasticstick::~elasticstick()
{
    e_os.close(); // FIXME
}

void elasticstick::setPPMap(ppmap& PPMap)
{
    PMap = &PPMap;
}

es_result elasticstick::initialize(const std::list<ppmap::position>& TopologicalPath)
{
    Stick.clear();
    for (list<ppmap::position>::const_iterator it = TopologicalPath.begin(); it != TopologicalPath.end(); ++it) {
        node new_node;
        new_node.X = it->X;
        new_node.Y = it->Y;
        new_node.BubbleRadius = PMap->nearestObstDistance(*it);
        Stick.push_back(new_node);
    }

    bool connected = false;
    while (!connected) {
        connected = true;
        for (list<node>::iterator it = Stick.begin(); it != Stick.end(); ++it) {
            if (it == Stick.begin()) continue;
            list<node>::iterator prev_it = it;
            --prev_it;
            if (PMap->posDist(*prev_it, *it) > prev_it->BubbleRadius) {
                connected = false;
                node new_node;
                new_node.X = (it->X + prev_it->X) / 2;
                new_node.Y = (it->Y + prev_it->Y) / 2;
                new_node.BubbleRadius = PMap->nearestObstDistance(new_node);
                if (new_node.BubbleRadius < ParamMinObstDist / 5) new_node.BubbleRadius = ParamMinObstDist / 5;
                if (new_node.BubbleRadius > 0) it = Stick.insert(it, new_node);
            }
        }
    }

    bool safe = true;
    for (list<node>::iterator it = Stick.begin(); it != Stick.end(); ++it) {
            // minimum obstacle distance
            ppmap::position nearest_obst_pos;
            double nearest_obst_dist = PMap->nearestObstDistance(*it, &nearest_obst_pos);
            if (nearest_obst_dist < ParamMinObstDist) {
                // the bubble is too near to an obstacle
                double displacement_value = ParamMinObstDist - nearest_obst_dist;
                double displacement_direction = PMap->posAngle(nearest_obst_pos, *it);
                double newx = it->X - displacement_value * cos(displacement_direction);
                double newy = it->Y - displacement_value * -sin(displacement_direction);
                if (!PMap->posSight(ppmap::position(newx, newy), *it)) {
                    // the old position and the new one cannot see each other, so we are
                    // passing through an obstacle
                    ppmap::position obst_pos = PMap->firstObstEncountered(*it, ppmap::position(newx, newy));
                    it->X = (nearest_obst_pos.X + obst_pos.X) / 2;
                    it->Y = (nearest_obst_pos.Y + obst_pos.Y) / 2;
                }
                else {
                    ppmap::position new_nearest_obst_pos;
                    double new_nearest_obst_dist = PMap->nearestObstDistance(ppmap::position(newx, newy), &new_nearest_obst_pos);
                    if (new_nearest_obst_dist < it->BubbleRadius) {
                        // in the new position the bubble radius would be lesser than in the current one
                        it->X = (it->X + newx) / 2;
                        it->Y = (it->Y + newy) / 2;
                    }
                    else if (new_nearest_obst_dist > 0) {
                        it->X = newx;
                        it->Y = newy;
                    }
                }
            }
            // update the bubble radius
            it->BubbleRadius = PMap->nearestObstDistance(*it);
	    if (it->BubbleRadius < ParamMinObstDist) safe = false;
    }
    return (safe ? ES_SAFE : ES_UNSAFE);	// FIXME e se fosse BROKEN?
}

double sincos2ang(double sinval, double cosval)
{
    double ang = acos(cosval);
    if (sinval < 0) ang = 2 * M_PI - ang;
    return ang;
}

es_result elasticstick::adjust(unsigned int IterationsCount)
{
    bool broken = false;
    bool unsafe = false;
	CriticalBubblePtr = 0;
    // there isn't any elastic band
    if (Stick.size() == 1) return ES_NONE;
    for (unsigned int i = 0; i < IterationsCount; i++) {
        // add new nodes (bubbles) to connect the band
        for (list<node>::iterator it = Stick.begin(); it != Stick.end(); ++it) {
            if (it == Stick.begin()) continue;
            list<node>::iterator prev_it = it;
            --prev_it;
            if (Stick.size() < 100 /* FIXME */ && PMap->posDist(*prev_it, *it) > prev_it->BubbleRadius) {
                node new_node;
                new_node.X = (it->X + prev_it->X) / 2;
                new_node.Y = (it->Y + prev_it->Y) / 2;
                new_node.BubbleRadius = PMap->nearestObstDistance(new_node);
                if (new_node.BubbleRadius < ParamMinObstDist / 5) new_node.BubbleRadius = ParamMinObstDist / 5;
                it = Stick.insert(it, new_node);
            }
        }
        // remove redunant nodes (bubbles)
        for (list<node>::iterator it = Stick.begin(); it != Stick.end(); ) {
            if (it == Stick.begin()) {
                ++it;
                continue;
            }
            list<node>::iterator prev_it = it;
            list<node>::iterator next_it = it;
            --prev_it;
            ++next_it;
            if (next_it == Stick.end()) break;
            if (PMap->nearestObstDistance(*it) == 0 || PMap->posDist(*prev_it, *next_it) < prev_it->BubbleRadius) {
                it = Stick.erase(it);
            }
            else ++it;
        }
        // move the nodes (bubbles) following the internal (contraction) and external (obstacles) forces
        for (list<node>::iterator it = Stick.begin(); it != Stick.end(); ++it) {
            if (it == Stick.begin()) {
                // if it's the first bubble (the starting position) we cannot move it
                // but the bubble radius has to be updated
                it->BubbleRadius = PMap->nearestObstDistance(*it);
                continue;
            }
            list<node>::iterator prev_it = it;
            list<node>::iterator next_it = it;
            --prev_it;
            ++next_it;
            if (next_it == Stick.end()) {
                // the last bubble cannot be moved
                it->BubbleRadius = PMap->nearestObstDistance(*it);
                break;
            }

            // check if the band is passing through an obstacle
            if (!broken && !PMap->posSight(*prev_it, *it)) {
				broken = true;
				CriticalBubblePtr = &*prev_it;
			}

            // prepare the force components;
            double fzx = 0, fzy = 0;

            // internal (contraction) force
            double bubble_distance = PMap->posDist(*it, *prev_it);
            double force_value = bubble_distance / ParamMinObstDist;
            fzx += force_value * (prev_it->X - it->X);
            fzy += force_value * (prev_it->Y - it->Y);
            bubble_distance = PMap->posDist(*it, *next_it);
            force_value = bubble_distance / ParamMinObstDist;
            fzx += force_value * (next_it->X - it->X);
            fzy += force_value * (next_it->Y - it->Y);

            // external (obstacles) force
            ppmap::position nearest_obst_pos;
            double nearest_obst_dist = PMap->nearestObstDistance(*it, &nearest_obst_pos);
            if (nearest_obst_dist > ParamMinObstDist * 2) force_value = 0;
            else force_value = ParamMinObstDist * 2 - nearest_obst_dist;
            double force_direction = PMap->posAngle(nearest_obst_pos, *it);
            fzx -= force_value * cos(force_direction);
            fzy -= force_value * -sin(force_direction);

            // we have to consider only the component of the force that is perpendicular
            // to the tangent versor of the path FIXME, forse si può semplificare qualche calcolo
            double tgx = next_it->X - prev_it->X;
            double tgy = next_it->Y - prev_it->Y;
            double tgmod = sqrt(tgx * tgx + tgy * tgy);
            double fzmod = sqrt(fzx * fzx + fzy * fzy);
            if (fzmod && tgmod) {
                double tgversx = tgx / tgmod;
                double tgversy = tgy / tgmod;
                double theta = sincos2ang(fzy / fzmod, fzx / fzmod) - sincos2ang(tgy / tgmod, tgx / tgmod);
                double ry = -fzmod * cos(M_PI / 2 - theta);
                fzx = ry * tgversy;
                fzy = ry * -tgversx;
            }

            // use the force
            double newx = it->X + 0.1 * fzx;
            double newy = it->Y + 0.1 * fzy;
            ppmap::position new_nearest_obst_pos;
            double new_nearest_obst_dist = PMap->nearestObstDistance(ppmap::position(newx, newy), &new_nearest_obst_pos);
            if (PMap->posSight(ppmap::position(newx, newy), *it)) {
				// we move the bubble only if it not cross an obstacle
			    if (new_nearest_obst_dist > nearest_obst_dist ) {
            	    it->X = newx;
	                it->Y = newy;
    	            nearest_obst_dist = new_nearest_obst_dist;
        	        nearest_obst_pos = new_nearest_obst_pos;
	                it->BubbleRadius = new_nearest_obst_dist;
				}
				else {
					it->X = (newx + it->X) / 2;
					it->Y = (newy + it->Y) / 2;
					nearest_obst_dist = new_nearest_obst_dist;
					nearest_obst_pos = new_nearest_obst_pos;
					it->BubbleRadius = new_nearest_obst_dist;
				}
            }

            if (!unsafe && !broken && it->BubbleRadius < ParamMinObstDist) {
                unsafe = true;
                CriticalBubblePtr = &*it;
            }
        }
        // stick force
        if (ParamMinSteeringRadius != 0) {
            for (list<node>::iterator it = Stick.begin(); it != Stick.end(); ++it) {
                list<node>::iterator prev_it = it;
                --prev_it;
                list<node>::iterator prev2_it = prev_it;
                --prev2_it;
                list<node>::iterator next_it = it;
                ++next_it;
                if (it == Stick.begin() || next_it == Stick.end()) continue;
                double ang2 = PMap->posAngle(*prev_it, *it);
                double ang1;
                if (prev_it != Stick.begin()) {
                    ang1 = PMap->posAngle(*prev2_it, *prev_it);
                }
                else {
                    ang1 = 0;
                }
                double ang = (ang1 - ang2) / 2;
                if (prev_it != Stick.begin()) {
                    if (ang > M_PI / 2) ang = M_PI + ang;
                    else if (ang < - M_PI / 2) ang = M_PI + ang;
                }

                double ang_force = ang;

                ppmap::position new_pos = PMap->rotatePosition(*it, *prev_it, ang_force);
                double fzx = new_pos.X - it->X;
                double fzy = new_pos.Y - it->Y;
                double fzmod = sqrt(fzx*fzx + fzy*fzy);
                if (fzmod) {
                    fzx = fzx * 1 * (fzx / fzmod);
                    fzy = fzy * 1* (fzy / fzmod);
                }

                // we have to consider only the component of the force that is perpendicular
                // to the tangent versor of the path FIXME, forse si può semplificare qualche calcolo
                double tgx = next_it->X - prev_it->X;
                double tgy = next_it->Y - prev_it->Y;
                double tgmod = sqrt(tgx * tgx + tgy * tgy);
                fzmod = sqrt(fzx * fzx + fzy * fzy);
                if (fzmod && tgmod) {
                    double tgversx = tgx / tgmod;
                    double tgversy = tgy / tgmod;
                    double theta = sincos2ang(fzy / fzmod, fzx / fzmod) - sincos2ang(tgy / tgmod, tgx / tgmod);
                    double ry = -fzmod * cos(M_PI / 2 - theta);
                    fzx = ry * tgversy;
                    fzy = ry * -tgversx;
                }

                it->X = it->X + 0.1 * fzx;
                it->Y = it->Y + 0.1 * fzy;
            }
            for (list<node>::reverse_iterator it = Stick.rbegin(); it != Stick.rend(); ++it) {
                list<node>::reverse_iterator prev_it = it;
                --prev_it;
                list<node>::reverse_iterator prev2_it = prev_it;
                --prev2_it;
                list<node>::reverse_iterator next_it = it;
                ++next_it;
                if (it == Stick.rbegin() || prev_it == Stick.rbegin() || next_it == Stick.rend()) continue;
                double ang1 = PMap->posAngle(*prev2_it, *prev_it);
                double ang2 = PMap->posAngle(*prev_it, *it);
                double ang = (ang1 - ang2) / 2;
                if (ang > M_PI / 2) ang = M_PI + ang;
                else if (ang < - M_PI / 2) ang = M_PI + ang;

                double ang_force = 10 * tan(M_PI / 2 * (ang / (2 * M_PI)));

                ppmap::position new_pos = PMap->rotatePosition(*it, *prev_it, ang_force);
                double fzx = new_pos.X - it->X;
                double fzy = new_pos.Y - it->Y;

                // we have to consider only the component of the force that is perpendicular
                // to the tangent versor of the path FIXME, forse si può semplificare qualche calcolo
                double tgx = next_it->X - prev_it->X;
                double tgy = next_it->Y - prev_it->Y;
                double tgmod = sqrt(tgx * tgx + tgy * tgy);
                double fzmod = sqrt(fzx * fzx + fzy * fzy);
                if (fzmod && tgmod) {
                    double tgversx = tgx / tgmod;
                    double tgversy = tgy / tgmod;
                    double theta = sincos2ang(fzy / fzmod, fzx / fzmod) - sincos2ang(tgy / tgmod, tgx / tgmod);
                    double ry = -fzmod * cos(M_PI / 2 - theta);
                    fzx = ry * tgversy;
                    fzy = ry * -tgversx;
                }

                it->X = it->X + 0.1 * fzx;
                it->Y = it->Y + 0.1 * fzy;
            }
        }
    }

	if (broken) return ES_BROKEN;
	else if (unsafe) return ES_UNSAFE;
	else return ES_SAFE;
}
