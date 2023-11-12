#ifndef ppmapH
#define ppmapH

#include "simple_fig_loader.h"

#include <vector>

class ppmap {
public:
    // structors
    ppmap();
    ~ppmap();

    // subclasses
    class position {
    public:
        position() : X(0), Y(0) {}
        position(double X, double Y) : X(X), Y(Y) {}
        double X, Y;
    };

    class configuration : public position {
    public:
        configuration() : position(0, 0), Theta(0) {}
        configuration(double X, double Y, double Theta) : position(X, Y), Theta(Theta) {}
        configuration(position Pos, double Theta) : position(Pos), Theta(Theta) {}
        double Theta;
    };

    // members
    bool UseObstBmps;
    bool UseObstSegments;
    bool** StaticObstBmp;
    bool** TemporaryObstBmp;
    std::vector<segment> StaticObstSegments;

    // parameters
    double ParamEpsilon;
    
    // functions
    position randomPosition();
    bool     posSight(const position& Pos1, const position& Pos2);
    double   posDist(const position& Pos1, const position& Pos2);
    bool     posAllowed(const position& Pos);
    double   posAngle(const position& Pos1, const position& Pos2);
    double   nearestObstDistance(const position& Pos, position* NearestObstPoint = 0);
    position firstObstEncountered(const position& StartPos, const position& EndPos);
    std::vector<position>
             gruyereTest(const position& Pos1, const position& Pos2);
    void     setLimits(int MinX, int MaxX, int MinY, int MaxY);
    void     allocObstBmps(double Quantization);
    double   getMinX();
    double   getMinY();
    double   getMaxX();
    double   getMaxY();
    double   getObstBmpsQuantization();
    int      getObstBmpsWidth();
    int      getObstBmpsHeight();
    position bmpToPos(int X, int Y);
    int      posToBmpX(const position& Pos);
    int      posToBmpY(const position& Pos);
    void     clearObstRay(const position& StartPos, const position& ObstPos);
    bool     sedgewickIntersect(double ax1, double ay1, double ax2, double ay2, double bx1, double by1, double bx2, double by2);
    double   posDistFromSegment(const position& Pos, const position& SegPoint1, const position& SegPoint2, position* NearestObstPoint = 0);
    position segmentsIntersection(const position& Seg1Point1, const position& Seg1Point2, const position& Seg2Point1, const position& Seg2Point2);
    position rotatePosition(const position& Pos, const position& Center, double Angle);
protected:
    // functions
    bool     checkObstBmpsPos(position Pos);
    bool     checkObstBmpsCell(int X, int Y);

    // members
    int MinX, MinY, MaxX, MaxY;
    int AllocBmpsWidth, AllocBmpsHeight;
    double ObstBmpsQuantization;  // each cell in the bitmap is ? units (centimeters?)
                                  // so to find the right cell it need to do (X - MinX) / MetricQuantization;
};

#endif
