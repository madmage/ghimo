#include <vector>
#include <QPainter>
#include "YarnObstAvoid_QImage.h"
using namespace std;

YarnObstAvoid_QImage::YarnObstAvoid_QImage() { }

#define SIGN(x) (x > 0 ? +1 : x < 0 ? -1 : 0)

void YarnObstAvoid_QImage::scan_line(int x1, int y1, int x2, int y2, int miny, vector<int>& minx, vector<int>& maxx) {
    int sx = x2 - x1, sy = y2 - y1, dx1, dy1, dx2, dy2, x, y, m, n, k, cnt;
    dx1 = SIGN(sx); dy1 = SIGN(sy);

    m = abs(sx); n = abs(sy); dx2 = dx1; dy2 = 0;
    if (m < n) { m = abs(sy); n = abs(sx); dx2 = 0; dy2 = dy1; }

    x = x1; y = y1; cnt = m + 1; k = n / 2;

    while (cnt--) {
        int i = y - miny;
        if (i >= 0 && (size_t) i < minx.size() && x < minx[i]) minx[i] = x;
        if (i >= 0 && (size_t) i < maxx.size() && x > maxx[i]) maxx[i] = x;

        k += n;
        if (k < m) { x += dx2; y += dy2; }
        else { k -= m; x += dx1; y += dy1; }
    }
}

// world -> raster
void YarnObstAvoid_QImage::w2r(double wx, double wy, double& rx, double& ry) {
    grid2d->mapWorldToImageD(wx, wy, rx, ry);
}

double YarnObstAvoid_QImage::collide(const gmp::Pose2d& robotPose, double enlarge) {
    double rw = params.robotSizeY, rwe = params.robotSizeY * enlarge;
    double rh = params.robotSizeX, rhe = params.robotSizeX * enlarge;
    gmp::Point2d p0(rh/2, rw/2), p1(-rh/2, rw/2), p2(-rh/2, -rw/2), p3(rh/2, -rw/2);
    gmp::Point2d p0e(rhe/2, rwe/2), p1e(-rhe/2, rwe/2), p2e(-rhe/2, -rwe/2), p3e(rhe/2, -rwe/2);
    p0.x -= params.robotCenterX; p1.x -= params.robotCenterX; p2.x -= params.robotCenterX; p3.x -= params.robotCenterX;
    p0.y -= params.robotCenterY; p1.y -= params.robotCenterY; p2.y -= params.robotCenterY; p3.y -= params.robotCenterY;
    p0e.x -= params.robotCenterX; p1e.x -= params.robotCenterX; p2e.x -= params.robotCenterX; p3e.x -= params.robotCenterX;
    p0e.y -= params.robotCenterY; p1e.y -= params.robotCenterY; p2e.y -= params.robotCenterY; p3e.y -= params.robotCenterY;
    p0 = p0.rotated(robotPose.theta); p1 = p1.rotated(robotPose.theta);
    p2 = p2.rotated(robotPose.theta); p3 = p3.rotated(robotPose.theta);
    p0e = p0e.rotated(robotPose.theta); p1e = p1e.rotated(robotPose.theta);
    p2e = p2e.rotated(robotPose.theta); p3e = p3e.rotated(robotPose.theta);
    p0.x += robotPose.x; p0.y += robotPose.y; p1.x += robotPose.x; p1.y += robotPose.y; p2.x += robotPose.x; p2.y += robotPose.y; p3.x += robotPose.x; p3.y += robotPose.y;
    p0e.x += robotPose.x; p0e.y += robotPose.y; p1e.x += robotPose.x; p1e.y += robotPose.y; p2e.x += robotPose.x; p2e.y += robotPose.y; p3e.x += robotPose.x; p3e.y += robotPose.y;
    double x0r, y0r, x1r, y1r, x2r, y2r, x3r, y3r;
    double x0re, y0re, x1re, y1re, x2re, y2re, x3re, y3re;
    w2r(p0.x, p0.y, x0r, y0r); w2r(p1.x, p1.y, x1r, y1r); w2r(p2.x, p2.y, x2r, y2r); w2r(p3.x, p3.y, x3r, y3r);
    w2r(p0e.x, p0e.y, x0re, y0re); w2r(p1e.x, p1e.y, x1re, y1re); w2r(p2e.x, p2e.y, x2re, y2re); w2r(p3e.x, p3e.y, x3re, y3re);

    if (x0r < 0 || y0r < 0 || x1r < 0 || y1r < 0 || x2r < 0 || y2r < 0 || x3r < 0 || y3r < 0) return false;
    if (x0r > 10000 || y0r > 10000 || x1r > 10000 || y1r > 10000 || x2r > 10000 || y2r > 10000 || x3r > 10000 || y3r > 10000) return false;
    if (x0re < 0 || y0re < 0 || x1re < 0 || y1re < 0 || x2re < 0 || y2re < 0 || x3re < 0 || y3re < 0) return false;
    if (x0re > 10000 || y0re > 10000 || x1re > 10000 || y1re > 10000 || x2re > 10000 || y2re > 10000 || x3re > 10000 || y3re > 10000) return false;

    int miny = min({ y0r, y1r, y2r, y3r });
    int maxy = max({ y0r, y1r, y2r, y3r });
    int minye = min({ y0re, y1re, y2re, y3re });
    int maxye = max({ y0re, y1re, y2re, y3re });

    vector<int> minx(maxy - miny, +INT_MAX);
    vector<int> maxx(maxy - miny, -INT_MAX);
    vector<int> minxe(maxye - minye, +INT_MAX);
    vector<int> maxxe(maxye - minye, -INT_MAX);

    scan_line(x0r, y0r, x1r, y1r, miny, minx, maxx);
    scan_line(x1r, y1r, x2r, y2r, miny, minx, maxx);
    scan_line(x2r, y2r, x3r, y3r, miny, minx, maxx);
    scan_line(x3r, y3r, x0r, y0r, miny, minx, maxx);
    scan_line(x0re, y0re, x1re, y1re, minye, minxe, maxxe);
    scan_line(x1re, y1re, x2re, y2re, minye, minxe, maxxe);
    scan_line(x2re, y2re, x3re, y3re, minye, minxe, maxxe);
    scan_line(x3re, y3re, x0re, y0re, minye, minxe, maxxe);

    double colliding = 0.0;  // 0.0 = not colliding, 0.5 = colliding only enlarged, 2 = colliding not enlarged

    // FIXME clamp minx and maxx w.r.t. [0, imgObstacles.width()] and i + miny w.r.t. [0, imgObstacles.height()]
    for (int i = 0; i < maxye - minye; i += 2) {
        int y = i + minye;
//cout << "y: " << y << "," << i << "," << minye << "," << imgObstacles.height() << endl;
        if (y < 0 || y >= imgObstacles.height()) continue;
//cout << "y: " << y << ", " << minye << "," << miny << "," << maxy << "," << maxye << endl;
        for (int x = minxe[i]; x <= maxxe[i]; x += 2) {
//cout << "x: " << x << "," << imgObstacles.width() << endl;
            if (x < 0 || x >= imgObstacles.width()) continue;
            QRgb pix = imgObstacles.pixel(x, y);
//cout << "[" << qRed(pix) << "," << qGreen(pix) << "," << qBlue(pix) << ": " << (qRed(pix) > 127) << "," << (qRed(pix) == qGreen(pix) == qBlue(pix) == 0) << "]";
//            imgObstaclesDebug.setPixel(x, y, qRgb(255, 0, 255));
            if (qRed(pix) > 127) { // || (qRed(pix) == 0 && qGreen(pix) == 0 && qBlue(pix) == 0)) {
//cout << "   x: " << x << ", " << minxe[i] << "," << minx[i] << "," << maxx[i] << "," << maxxe[i] << endl;
//                imgObstaclesDebug.setPixel(x, y, qRgb(255, 255, 255));
                if (y >= miny && y <= maxy && x >= minx[i] && x <= maxx[i]) {
                    colliding = 1.0;
                    break;
                }
                else {
                    colliding = 0.5;
                }
            }
        }
        if (colliding == 1.0) break;
    }

    if (debugMode == 1) {
        QPainter painter;
        painter.begin(&imgObstaclesDebug);
        QPen pen;
        pen.setWidth(1);
        pen.setColor(colliding == 1.0 ? qRgb(127, 0, 0) : colliding == 0.5 ? qRgb(127, 70, 0) : qRgb(0, 127, 0));
        painter.setPen(pen);
        painter.drawLine(x0re, y0re, x1re, y1re);
        painter.drawLine(x1re, y1re, x2re, y2re);
        painter.drawLine(x2re, y2re, x3re, y3re);
        painter.drawLine(x3re, y3re, x0re, y0re);
        painter.drawLine(x0r, y0r, x1r, y1r);
        painter.drawLine(x1r, y1r, x2r, y2r);
        painter.drawLine(x2r, y2r, x3r, y3r);
        painter.drawLine(x3r, y3r, x0r, y0r);
        painter.end();
    }

    return colliding;
}
