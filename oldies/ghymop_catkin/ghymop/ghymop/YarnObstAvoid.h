#ifndef GHYMOP_YARNOBSTAVOID_H
#define GHYMOP_YARNOBSTAVOID_H

#include <vector>
#include <cmath>
#include <cstddef>
#include <climits>

namespace gmp {

inline void quickPredictNextPose(double curx, double cury, double curtheta, double v, double w, double deltat, double& nextx, double& nexty, double& nexttheta) {
    nextx = curx + v * deltat * std::cos(curtheta + w * deltat / 2);
    nexty = cury + v * deltat * std::sin(curtheta + w * deltat / 2);
    nexttheta = curtheta + w * deltat;
}

struct Point2d {
    double x, y;
    Point2d(double x, double y) : x(x), y(y) { }
    Point2d rotated(double angle);
};

struct Pose2d {
    Pose2d(double x, double y, double theta) : x(x), y(y), theta(theta) { }
    double x, y, theta;
    double collide = 0.0;
    double poseScore = 0.0;
};

struct Trajectory2d {
    std::vector<Pose2d> poses;
    size_t trajCollisionIndex = UINT_MAX;
    double robotSpeed, robotJog;
    double trajScore = 0.0;
};

struct Trajectory2dSet : public std::vector<Trajectory2d> { };

class YarnObstAvoid2d {
public:
    YarnObstAvoid2d();

    struct Params {
        // *Steps = 0 means only, e.g., the ctrlSpeed
        // *Steps = 1 means, e.g., ctrlSpeed, ctrlSpeed + deltaSpeed, ctrlSpeed - deltaSpeed
        // *Steps = 2 means, e.g., ctrlSpeed, ctrlSpeed + deltaSpeed / 2, ctrlSpeed + deltaSpeed, ctrlSpeed - deltaSpeed / 2, ctrlSpeed - deltaSpeed
        double deltaSpeed = 0.2, deltaJog = 0.4;
        int speedSteps = 2, jogSteps = 4;
        double trajDuration = 2.0;
        int trajSteps = 10;
        double robotSizeX = 0.84, robotSizeY = 0.525;
        double robotCenterX = 0.59, robotCenterY = 0.0;
    } params;

    void obstAvoid(const Pose2d& robotPose, double ctrlSpeed, double ctrlJog, double& correctedSpeed, double& correctedJog);
    void obstAvoid(const Pose2d& robotPose, const Pose2d& targetPose, double curSpeed, double curJog, double& yarnSpeed, double& yarnJog);
    Trajectory2dSet getLastTrajSet() { return trajSet; }

protected:
    virtual double collide(const Pose2d& robotPose, double enlarge) = 0;

    Trajectory2dSet trajSet;
};

}

#endif // GHYMOP_YARNOBSTAVOID_H
