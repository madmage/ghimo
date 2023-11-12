#include <vector>
using std::vector;
#include <map>
using std::map;
#include <cmath>
using std::sqrt;
using std::asin;
#include <fstream>      // FIXME
using std::ofstream;
using std::endl;


#include "utilz.h"
#include "ppmap.h"

ofstream p_os; // FIXME

ppmap::ppmap() :
    UseObstBmps(false),
    UseObstSegments(false),
    StaticObstBmp(0),
    TemporaryObstBmp(0),
    AllocBmpsWidth(0),
    AllocBmpsHeight(0),
    ObstBmpsQuantization(0),
    ParamEpsilon(10),
    MinX(0), MaxX(0), MinY(0), MaxY(0)
{
#ifdef __BORLANDC__
    p_os.open("ppmap-log.txt");
#else
	p_os.open("pathde/logs/ppmap-log");
#endif
}

ppmap::~ppmap()
{
    // avoid memory leaks! ;-D
    if (StaticObstBmp) {
        for (int i = 0; i < AllocBmpsWidth; i++) delete[] StaticObstBmp[i];
        delete[] StaticObstBmp;
    }
    if (TemporaryObstBmp) {
        for (int i = 0; i < AllocBmpsWidth; i++) delete[] TemporaryObstBmp[i];
        delete[] TemporaryObstBmp;
    }
    p_os.close();    // FIXME
}

double ppmap::posDist(const position& Pos1, const position& Pos2)
{
    int dx = (int) (Pos1.X - Pos2.X);
    int dy = (int) (Pos1.Y - Pos2.Y);
    return sqrt((double) (dx * dx + dy * dy));
}

double ppmap::posAngle(const position& Pos1, const position& Pos2)
{
    int lx = (int) (Pos1.X - Pos2.X);
    int ly = (int) -(Pos1.Y - Pos2.Y);

    double r = sqrt((double) (lx*lx + ly*ly));
    if (r == 0) r = 1;
    double ang = asin(ly / r);
    if (ang > 0) {
        if (lx < 0) return (M_PI - ang);
        else return ang;
    }
    else {
        if (lx < 0) return (M_PI - ang);
        else return (2 * M_PI + ang);
    }
}

ppmap::position ppmap::rotatePosition(const position& Pos, const position& Center, double Angle)
{
    double dx = Pos.X - Center.X;
    double dy = Pos.Y - Center.Y;
    double cr = cos(Angle);
    double sr = -sin(Angle);
    position retval;
    retval.X = (dx * cr) - (dy * sr) + Center.X;
    retval.Y = (dx * sr) + (dy * cr) + Center.Y;
    return retval;
}

double ppmap::nearestObstDistance(const ppmap::position& Pos, position* NearestObstPoint)   // NearestObstPoint = 0
{
    const int MAX_DIST = MaxX + MaxY - MinX - MinY;
    double min_distance = MAX_DIST;

    if (UseObstSegments) {
        if (fabs(Pos.X - MaxX) < min_distance) {
            if (NearestObstPoint) *NearestObstPoint = position(MaxX, Pos.Y);
            min_distance = fabs(Pos.X - MaxX);
        }
        if (fabs(Pos.X - MinX) < min_distance) {
            if (NearestObstPoint) *NearestObstPoint = position(MinX, Pos.Y);
            min_distance = fabs(Pos.X - MinX);
        }
        if (fabs(Pos.Y - MaxY) < min_distance) {
            if (NearestObstPoint) *NearestObstPoint = position(Pos.X, MaxY);
            min_distance = fabs(Pos.Y - MaxY);
        }
        if (fabs(Pos.Y - MinY) < min_distance) {
            if (NearestObstPoint) *NearestObstPoint = position(Pos.X, MinY);
            min_distance = fabs(Pos.Y - MinY);
        }

        for (unsigned int i = 0; i < StaticObstSegments.size(); i++) {
            position min_dist_obst;
            double distance = posDistFromSegment(Pos, position(StaticObstSegments[i].X1, StaticObstSegments[i].Y1),
                                                      position(StaticObstSegments[i].X2, StaticObstSegments[i].Y2),
                                                      &min_dist_obst);
            if (distance < min_distance) {
                min_distance = distance;
                if (NearestObstPoint) *NearestObstPoint = min_dist_obst;
            }
        }
    }

    if (UseObstBmps && (StaticObstBmp || TemporaryObstBmp)) {
        if (checkObstBmpsPos(Pos)) {
            // the position is IN an obstacle
            if (NearestObstPoint) *NearestObstPoint = Pos;
            return 0;
        }
        int r = 1;
        int startx = posToBmpX(Pos);
        int starty = posToBmpY(Pos);
        int found_countdown = -1;
        if (min_distance < MAX_DIST) found_countdown = (int) (min_distance / ObstBmpsQuantization);
        while (found_countdown == -1 || found_countdown > 0) {
            // we start from the cell at the left of the last "circle"
            for (int d = 0; d < 4; d++) {
                int dx, dy, x, y;
                switch (d) {
                    case 0: x = (int) startx - r; y = (int) starty; 	  dx = 1;  dy = -1; break;
                    case 1: x = (int) startx; 	  y = (int) starty - r;   dx = 1;  dy = 1; break;
                    case 2: x = (int) startx + r; y = (int) starty; 	  dx = -1; dy = 1; break;
                    case 3: x = (int) startx;  	  y = (int) starty + r;   dx = -1; dy = -1; break;
                }
                for (int i = 0; i < r; i++) {
                    if (checkObstBmpsCell(x + i * dx, y + i * dy)) {
                        double distance = posDist(Pos, bmpToPos(x + i * dx, y + i * dy));
                        if (distance < min_distance) {
                            // this is a better minimum distance
                            min_distance = distance;
                            if (NearestObstPoint) *NearestObstPoint = bmpToPos(x + i * dx, y + i * dy);
                            // if it's the first obstacle we find, we start the countdown
                            if (found_countdown == -1) found_countdown = (int) ((double) r * 0.2929 + 1);
                        }
                    }
                }
            }
            if (found_countdown != -1) found_countdown--;
            r++;
            if (r > MAX_DIST) break;    // defensive programming
        }
    }
    return min_distance;
}

bool ppmap::posAllowed(const position& Pos)
{
    if (Pos.X < MinX || Pos.X > MaxX || Pos.Y < MinY || Pos.Y > MaxY) return false;
    if (UseObstBmps) {
        if (checkObstBmpsPos(Pos)) return false;
        if (checkObstBmpsPos(position(Pos.X + ObstBmpsQuantization, Pos.Y))) return false;
    }
    if (UseObstSegments) {
        double min_distance = nearestObstDistance(Pos);
        if (min_distance <= 10) return false;      // FIXME
    }
    return true;
}

// Sedgewick's algorithm (segments intersection)
int sedgewick_ccw(double p0x, double p0y, double p1x, double p1y, double p2x, double p2y)
{
    int dx1, dx2, dy1, dy2;
    dx1 = (int) (p1x - p0x); dy1 = (int) (p1y - p0y);
    dx2 = (int) (p2x - p0x); dy2 = (int) (p2y - p0y);
    if (dx1*dy2 > dy1*dx2) return +1;
    if (dx1*dy2 < dy1*dx2) return -1;
    if ((dx1*dx2 < 0) || (dy1*dy2 < 0)) return -1;
    if ((dx1*dx1+dy1*dy1) < (dx2*dx2+dy2*dy2)) return +1;
    return 0;
}

// Sedgewick's algorithm (segments intersection)
bool ppmap::sedgewickIntersect(double ax1, double ay1, double ax2, double ay2, double bx1, double by1, double bx2, double by2)
{
    return ((sedgewick_ccw(ax1, ay1, ax2, ay2, bx1, by1)
            *sedgewick_ccw(ax1, ay1, ax2, ay2, bx2, by2)) <= 0)
        && ((sedgewick_ccw(bx1, by1, bx2, by2, ax1, ay1)
            *sedgewick_ccw(bx1, by1, bx2, by2, ax2, ay2)) <= 0);
}

bool ppmap::posSight(const position& APos1, const position& APos2)
{
	position Pos1, Pos2;
    if (APos1.X < APos2.X || (APos1.X == APos2.X && APos1.Y < APos2.Y)) {
		Pos1 = APos1;
		Pos2 = APos2;
	}
	else {
		Pos1 = APos2;
		Pos2 = APos1;
	}
    if (UseObstSegments) {
        bool any_intersection = false;
        for (unsigned int i = 0; i < StaticObstSegments.size(); i++) {
            if (sedgewickIntersect(StaticObstSegments[i].X1, StaticObstSegments[i].Y1, StaticObstSegments[i].X2, StaticObstSegments[i].Y2,
                                    Pos1.X, Pos1.Y, Pos2.X, Pos2.Y))
            {
                any_intersection = true;
                break;
            }
        }
        if (any_intersection) return false;
    }
    if (UseObstBmps) {
        int vx = (Pos1.X < Pos2.X ? 1 : -1), vy = (Pos1.Y < Pos2.Y ? 1 : -1);
        unsigned int stepsx = (unsigned int) (fabs(Pos1.X - Pos2.X) / ObstBmpsQuantization);
        unsigned int stepsy = (unsigned int) (fabs(Pos1.Y - Pos2.Y) / ObstBmpsQuantization);

        unsigned int steps = (stepsx > stepsy ? stepsx : stepsy);
        if (!steps) steps = 1;
        double incx = (double) stepsx / steps;
        double incy = (double) stepsy / steps;

        double x = posToBmpX(Pos1), y = posToBmpY(Pos1);
        for (unsigned int i = 0; i < steps; i++) {
            if (checkObstBmpsCell((int) x, (int) y)) return false;
            if (checkObstBmpsCell((int) x + 1, (int) y)) return false;	// FIXME?
            x += incx * vx;
            y += incy * vy;
        }
        return true;
    }
    return true;
}

vector<ppmap::position> ppmap::gruyereTest(const position& Pos1, const position& Pos2)
{
    vector<position> retval;
    map<double, position> map_intersect;
    if (UseObstBmps) {
        int vx = (Pos1.X < Pos2.X ? 1 : -1), vy = (Pos1.Y < Pos2.Y ? 1 : -1);
        unsigned int stepsx = (unsigned int) (fabs(Pos1.X - Pos2.X) / ObstBmpsQuantization);
        unsigned int stepsy = (unsigned int) (fabs(Pos1.Y - Pos2.Y) / ObstBmpsQuantization);

        unsigned int steps = (stepsx > stepsy ? stepsx : stepsy);
        if (!steps) steps = 1;
        double incx = (double) stepsx / steps;
        double incy = (double) stepsy / steps;

        position last_position = Pos1;
        bool old_free_space = checkObstBmpsPos(Pos1);
        bool free_space = old_free_space;

        double x = posToBmpX(Pos1), y = posToBmpY(Pos1);
        for (unsigned int i = 0; i < steps; i++) {
            free_space = checkObstBmpsCell((int) x, (int) y);

            if ((old_free_space == true && free_space == false) || (old_free_space == false && free_space == true)) {
                position p = bmpToPos((int) x, (int) y);
                double posdist = posDist(Pos1, p);
                map_intersect[posdist] = p;
            }

            old_free_space = free_space;
            x += incx * vx;
            y += incy * vy;
        }
    }
    if (UseObstSegments) {
        for (unsigned int i = 0; i < StaticObstSegments.size(); i++) {
            if (sedgewickIntersect(Pos1.X, Pos1.Y, Pos2.X, Pos2.Y,
                                    StaticObstSegments[i].X1, StaticObstSegments[i].Y1,
                                    StaticObstSegments[i].X2, StaticObstSegments[i].Y2)
            ) {
                position posint = segmentsIntersection(Pos1, Pos2,
                                        position(StaticObstSegments[i].X1, StaticObstSegments[i].Y1),
                                        position(StaticObstSegments[i].X2, StaticObstSegments[i].Y2));
                double posdist = posDist(Pos1, posint);
                map_intersect[posdist] = posint;
            }
        }
    }
    map_intersect[posDist(Pos1, Pos2)] = Pos2;
    position old_position = Pos1;
    for (map<double, position>::iterator it = map_intersect.begin(); it != map_intersect.end(); ++it) {
        retval.push_back(position((it->second.X + old_position.X) / 2, (it->second.Y + old_position.Y) / 2));
        old_position = it->second;
    }
    return retval;
}

void ppmap::clearObstRay(const position& StartPos, const position& ObstPos)
{
    if (UseObstBmps && TemporaryObstBmp) {
        int vx = (StartPos.X < ObstPos.X ? 1 : -1), vy = (StartPos.Y < ObstPos.Y ? 1 : -1);
        unsigned int stepsx = (unsigned int) (fabs(StartPos.X - ObstPos.X) / ObstBmpsQuantization);
        unsigned int stepsy = (unsigned int) (fabs(StartPos.Y - ObstPos.Y) / ObstBmpsQuantization);

        unsigned int steps = (stepsx > stepsy ? stepsx : stepsy);
        if (!steps) steps = 1;
        double incx = (double) stepsx / steps;
        double incy = (double) stepsy / steps;

        // erase obstacles in the area of the ray until we reach the obstacle seen
        double x = posToBmpX(StartPos), y = posToBmpY(StartPos);
        for (unsigned int i = 0; i < steps; i++) {
            if (x > 0 && x < AllocBmpsWidth && y > 0 && y < AllocBmpsHeight) {
                TemporaryObstBmp[(int) x][(int) y] = false;
            }
            x += incx * vx;
            y += incy * vy;
        }

        /*// set the obstacle seen
		int ax = posToBmpX(ObstPos);
		int ay = posToBmpY(ObstPos);
        if (ax > 0 && ax < AllocBmpsWidth && ay > 0 && ay < AllocBmpsHeight) {
                TemporaryObstBmp[(int) ax][(int) ay] = true;
        }*/
    }
}

ppmap::position ppmap::firstObstEncountered(const position& StartPos, const position& EndPos)
{
    position first_obstacle = EndPos;
    double min_distance = posDist(StartPos, EndPos);
    if (UseObstSegments) {
       for (unsigned int i = 0; i < StaticObstSegments.size(); i++) {
            if (sedgewickIntersect(StartPos.X, StartPos.Y, EndPos.X, EndPos.Y,
                                    StaticObstSegments[i].X1, StaticObstSegments[i].Y1,
                                    StaticObstSegments[i].X2, StaticObstSegments[i].Y2)
            ) {
                position posint = segmentsIntersection(StartPos, EndPos,
                                        position(StaticObstSegments[i].X1, StaticObstSegments[i].Y1),
                                        position(StaticObstSegments[i].X2, StaticObstSegments[i].Y2));
                double posdist = posDist(StartPos, posint);
                if (posdist < min_distance) {
                    first_obstacle = posint;
                    min_distance = posdist;
                }
            }
        }
     }
    if (UseObstBmps) {
        int vx = (StartPos.X < EndPos.X ? 1 : -1), vy = (StartPos.Y < EndPos.Y ? 1 : -1);
        unsigned int stepsx = (unsigned int) (fabs(StartPos.X - EndPos.X) / ObstBmpsQuantization);
        unsigned int stepsy = (unsigned int) (fabs(StartPos.Y - EndPos.Y) / ObstBmpsQuantization);

        unsigned int steps = (stepsx > stepsy ? stepsx : stepsy);
        if (!steps) steps = 1;
        double incx = (double) stepsx / steps;
        double incy = (double) stepsy / steps;

        double x = posToBmpX(StartPos), y = posToBmpY(StartPos);
        for (unsigned int i = 0; i < steps; i++) {
            if (checkObstBmpsCell((int) x, (int) y)) {
                double this_dist = posDist(StartPos, bmpToPos((int) x, (int) y));
                if (this_dist < min_distance) {
                    min_distance = this_dist;
                    first_obstacle = bmpToPos((int) x, (int) y);
                }
            }
            x += incx * vx;
            y += incy * vy;
        }
    }
    return first_obstacle;
}

ppmap::position ppmap::randomPosition()
{
    return position(myrandom(MaxX - MinX) + MinX, myrandom(MaxY - MinY) + MinY);
}

void ppmap::setLimits(int MinX, int MinY, int MaxX, int MaxY)
{
    this->MinX = MinX;
    this->MaxX = MaxX;
    this->MinY = MinY;
    this->MaxY = MaxY;
}

void ppmap::allocObstBmps(double Quantization)
{
    if (StaticObstBmp) {
        for (int i = 0; i < AllocBmpsWidth; i++) delete[] StaticObstBmp[i];
        delete[] StaticObstBmp;
    }
    if (TemporaryObstBmp) {
        for (int i = 0; i < AllocBmpsWidth; i++) delete[] TemporaryObstBmp[i];
        delete[] TemporaryObstBmp;
    }
    ObstBmpsQuantization = Quantization;
    AllocBmpsWidth = (int) ((MaxX - MinX) / ObstBmpsQuantization);
    AllocBmpsHeight = (int) ((MaxY - MinY) / ObstBmpsQuantization);
    StaticObstBmp = new bool*[AllocBmpsWidth];
    TemporaryObstBmp = new bool*[AllocBmpsWidth];
    for (int i = 0; i < AllocBmpsWidth; i++) {
        StaticObstBmp[i] = new bool[AllocBmpsHeight];
        TemporaryObstBmp[i] = new bool[AllocBmpsHeight];
    }
    for (int x = 0; x < AllocBmpsWidth; x++) {
        for (int y = 0; y < AllocBmpsHeight; y++) {
            StaticObstBmp[x][y] = false;
            TemporaryObstBmp[x][y] = false;
        }
    }
}

/*
    "GET" FUNCTIONS
*/

double ppmap::getObstBmpsQuantization()
{
    return ObstBmpsQuantization;
}

int ppmap::getObstBmpsWidth()
{
    return AllocBmpsWidth;
}

int ppmap::getObstBmpsHeight()
{
    return AllocBmpsHeight;
}

double ppmap::getMinX()
{
    return MinX;
}

double ppmap::getMinY()
{
    return MinY;
}

double ppmap::getMaxX()
{
    return MaxX;
}

double ppmap::getMaxY()
{
    return MaxY;
}

/*
    PROTECTED FUNCTIONS
*/

double ppmap::posDistFromSegment(const position& Pos, const position& SegPoint1, const position& SegPoint2, position* NearestObstPoint) // NearestObstPoint = 0
{
    double distance;
    double vx = SegPoint2.X - SegPoint1.X;
    double vy = SegPoint2.Y - SegPoint1.Y;
    double wx = Pos.X - SegPoint1.X;
    double wy = Pos.Y - SegPoint1.Y;
    double c1 = (wx * vx + wy * vy);
    if (c1 <= 0) {
        distance = posDist(Pos, SegPoint1);
        if (NearestObstPoint) *NearestObstPoint = SegPoint1;
    }
    else {
        double c2 = (vx * vx + vy * vy);
        if (c2 <= c1) {
            distance = posDist(Pos, SegPoint2);
            if (NearestObstPoint) *NearestObstPoint = SegPoint2;
        }
        else {
            double b = c1 / c2;
            double bx = SegPoint1.X + b * vx;
            double by = SegPoint1.Y + b * vy;
            double dx = Pos.X - bx;
            double dy = Pos.Y - by;
            distance = sqrt(dx * dx + dy * dy);
            if (NearestObstPoint) *NearestObstPoint = position(bx, by);
        }
    }
    return distance;
}

bool ppmap::checkObstBmpsPos(position Pos)
{
    if (Pos.X > MaxX || Pos.X < MinX || Pos.Y > MaxY || Pos.Y < MinY) return true;
    int mx = posToBmpX(Pos);
    int my = posToBmpY(Pos);
    return checkObstBmpsCell(mx, my);
}

bool ppmap::checkObstBmpsCell(int X, int Y)
{
    if (X > AllocBmpsWidth - 1 || X < 0 || Y > AllocBmpsHeight - 1 || Y < 0) return true;
    if (StaticObstBmp && StaticObstBmp[X][Y]) return true;
    else if (TemporaryObstBmp && TemporaryObstBmp[X][Y]) return true;
    else return false;
}

ppmap::position ppmap::bmpToPos(int X, int Y)
{
    return position(X * ObstBmpsQuantization + MinX, Y * ObstBmpsQuantization + MinY);
}

int ppmap::posToBmpX(const position& Pos)
{
    return (int) ((Pos.X - MinX) / ObstBmpsQuantization);
}

int ppmap::posToBmpY(const position& Pos)
{
    return (int) ((Pos.Y - MinY) / ObstBmpsQuantization);
}

ppmap::position ppmap::segmentsIntersection(const position& Seg1Point1, const position& Seg1Point2, const position& Seg2Point1, const position& Seg2Point2)
{
    // direction vectors (actually not "positions", but vectors) and diffpos vector
    position d0(Seg1Point2.X - Seg1Point1.X, Seg1Point2.Y - Seg1Point1.Y);
    position d1(Seg2Point2.X - Seg2Point1.X, Seg2Point2.Y - Seg2Point1.Y);
    position c(Seg2Point1.X - Seg1Point1.X, Seg2Point1.Y - Seg1Point1.Y);
    // s and return value
    if (-d0.X * d1.Y + d1.X * d0.Y == 0) return position(0, 0); //FIXME horrible!
    double s = (-c.X * d1.Y + d1.X * c.Y) / (-d0.X * d1.Y + d1.X * d0.Y);
    return position(Seg1Point1.X + d0.X * s, Seg1Point1.Y + d0.Y * s);
}
