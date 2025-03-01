#ifndef YARNOBSTAVOID_QIMAGE_H
#define YARNOBSTAVOID_QIMAGE_H

#include <vector>
#include <QImage>
#include <trav_library/grid2d.h>
#include <ghymop/YarnObstAvoid.h>

class YarnObstAvoid_QImage : public gmp::YarnObstAvoid2d {
public:
    YarnObstAvoid_QImage();

    void setDebugMode(int debugMode) { this->debugMode = debugMode; }

    void setQImage(const QImage& imgObstacles) { 
        this->imgObstacles = imgObstacles; 
        this->imgObstaclesDebug = this->imgObstacles; 
        this->imgWidth = imgObstacles.width(); 
        this->imgHeight = imgObstacles.height(); 
    }
    void setGrid2D(trav::Grid2D* grid2d) { this->grid2d = grid2d; }
    QImage getQImage() { return imgObstaclesDebug; }

protected:
    double collide(const gmp::Pose2d& robotPose, double enlarge);
    void w2r(double wx, double wy, double& rx, double& ry);
    void scan_line(int x1, int y1, int x2, int y2, int miny, std::vector<int>& minx, std::vector<int>& maxx);
    QImage imgObstacles;
    QImage imgObstaclesDebug;
    trav::Grid2D* grid2d;
    int imgWidth = 0, imgHeight = 0;
    int debugMode = 0;
};

#endif // YARNOBSTAVOID_QIMAGE_H
