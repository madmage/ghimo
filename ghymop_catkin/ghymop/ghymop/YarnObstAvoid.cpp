#include "YarnObstAvoid.h"
#include <iostream>
#include <algorithm>
#include <cfloat>
using namespace std;

namespace gmp {

YarnObstAvoid2d::YarnObstAvoid2d() { }

Point2d Point2d::rotated(double angle) {
    return Point2d(this->x * cos(angle) - this->y * sin(angle), this->x * sin(angle) + this->y * cos(angle));
}

//void YarnObstAvoid2d::obstAvoid(const Pose2d& robotPose, double curSpeed, double curJog, double ctrlSpeed, double ctrlJog, double& corrSpeed, double& corrJog) {
void YarnObstAvoid2d::obstAvoid(const Pose2d& robotPose, double ctrlSpeed, double ctrlJog, double& corrSpeed, double& corrJog) {
    // speedSteps = 0 means only the ctrlSpeed
    // speedSteps = 1 means ctrlSpeed, ctrlSpeed + deltaSpeed, ctrlSpeed - deltaSpeed
    // speedSteps = 2 means ctrlSpeed, ctrlSpeed + deltaSpeed / 2, ctrlSpeed + deltaSpeed, ctrlSpeed - deltaSpeed / 2, ctrlSpeed - deltaSpeed
    trajSet.clear();
    Trajectory2d traj;
    double vm = params.deltaSpeed / params.speedSteps, wm = params.deltaJog / params.jogSteps;
    vm = ctrlSpeed / params.speedSteps;
    for (int vv = 0; vv >= -params.speedSteps; vv--) {
        for (int ww = -params.jogSteps; ww <= params.jogSteps; ww++) {
            traj.poses.clear();
            size_t trajCollisionIndex = UINT_MAX;
            double bestScore = -DBL_MAX;
            Pose2d p = robotPose;
            traj.robotSpeed = ctrlSpeed + vv * vm;
            traj.robotJog = ctrlJog + ww * wm;
            double curCollide = 0.0;
            for (size_t i = 0; i < params.trajSteps; i++) {
                Pose2d pose = p;
                double enlarge = 1.0 + min(0.4 / (params.trajSteps / 2) * i, 0.4);
                pose.collide = max(curCollide, collide(pose, enlarge));
                curCollide = pose.collide;
                pose.poseScore = -((params.trajSteps - i) * 100 + abs(vv) * 10 + abs(ww) + pose.collide * 10);
                bestScore = max(bestScore, pose.poseScore);
                traj.poses.push_back(pose);
                if (pose.collide == 1.0 && trajCollisionIndex == UINT_MAX) {
                    trajCollisionIndex = i;
                    break;
                }
                double x, y, th;
                quickPredictNextPose(p.x, p.y, p.theta, traj.robotSpeed, traj.robotJog, params.trajDuration / params.trajSteps, x, y, th);
                p.x = x; p.y = y; p.theta = th;
            }
            traj.trajCollisionIndex = trajCollisionIndex;
            traj.trajScore = bestScore;
//cout << "Traj " << traj.robotSpeed << ", " << traj.robotJog << ", collision at " << trajCollisionIndex << endl;
            trajSet.push_back(traj);
        }
    }
    sort(trajSet.begin(), trajSet.end(), [](const Trajectory2d& traj1, const Trajectory2d& traj2) { return traj1.trajScore > traj2.trajScore; });
    const Trajectory2d& btraj = trajSet[0];
    if (btraj.trajScore > -300) {
        corrSpeed = trajSet[0].robotSpeed;
        corrJog = trajSet[0].robotJog;
    }
    else {
        corrSpeed = corrJog = 0.0;
    }
}

void YarnObstAvoid2d::obstAvoid(const Pose2d& robotPose, const Pose2d& targetPose, double curSpeed, double curJog, double& yarnSpeed, double& yarnJog) {
    // speedSteps = 0 means only the ctrlSpeed
    // speedSteps = 1 means ctrlSpeed, ctrlSpeed + deltaSpeed, ctrlSpeed - deltaSpeed
    // speedSteps = 2 means ctrlSpeed, ctrlSpeed + deltaSpeed / 2, ctrlSpeed + deltaSpeed, ctrlSpeed - deltaSpeed / 2, ctrlSpeed - deltaSpeed
    trajSet.clear();
    Trajectory2d traj;
    double vm = params.deltaSpeed / params.speedSteps, wm = params.deltaJog / params.jogSteps;
    //vm = ctrlSpeed / params.speedSteps;
    for (int vv = params.speedSteps; vv >= 0; vv--) {
        for (int ww = -params.jogSteps; ww <= params.jogSteps; ww++) {
            traj.poses.clear();
            size_t trajCollisionIndex = UINT_MAX;
            double bestScore = -DBL_MAX;
            Pose2d p = robotPose;
            traj.robotSpeed = curSpeed + vv * vm;
            traj.robotJog = curJog + ww * wm;
            double curCollide = 0.0;
            for (size_t i = 0; i < params.trajSteps; i++) {
                Pose2d pose = p;
                double enlarge = 1.0 + min(0.3 / (params.trajSteps / 2) * i, 0.3);
                pose.collide = max(curCollide, collide(pose, enlarge));
                curCollide = pose.collide;
                double coll = -pose.collide * 10;
                double far = i * 100;
                double dist = 10 - min(10.0, sqrt((pose.x - targetPose.x) * (pose.x - targetPose.x) + (pose.y - targetPose.y) * (pose.y - targetPose.y)));
                //double dir = fabs(atan2(
                pose.poseScore = coll + far + dist;
                bestScore = max(bestScore, pose.poseScore);
                traj.poses.push_back(pose);
                if (pose.collide == 1.0 && trajCollisionIndex == UINT_MAX) {
                    trajCollisionIndex = i;
                    break;
                }
                double x, y, th;
                quickPredictNextPose(p.x, p.y, p.theta, traj.robotSpeed, traj.robotJog, params.trajDuration / params.trajSteps, x, y, th);
                p.x = x; p.y = y; p.theta = th;
            }
            traj.trajCollisionIndex = trajCollisionIndex;
            traj.trajScore = bestScore;
//cout << "Traj " << traj.robotSpeed << ", " << traj.robotJog << ", collision at " << trajCollisionIndex << endl;
            trajSet.push_back(traj);
        }
    }
    sort(trajSet.begin(), trajSet.end(), [](const Trajectory2d& traj1, const Trajectory2d& traj2) { return traj1.trajScore > traj2.trajScore; });
    const Trajectory2d& btraj = trajSet[0];
    if (btraj.trajScore > -300) {
        yarnSpeed = trajSet[0].robotSpeed;
        yarnJog = trajSet[0].robotJog;
    }
    else {
        yarnSpeed = yarnJog = 0.0;
    }
}
}
