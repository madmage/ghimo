#include <cmath>
#include <ghimo/geometry2d.h>

namespace ghimo {

using namespace std;

double angle_diff(double a0, double a1) {
    return atan2(sin(a0 - a1), cos(a0 - a1));
}

}
