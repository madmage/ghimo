#include <cmath>
#include <iostream>

using namespace std;


double tests[][3] = 
  { { 0.0, 0.0, 0.0 }, { M_PI, 0, M_PI } };

double angle_diff(double a, double b) {
  return atan2(sin(a - b), cos(a - b));
}

int main() {
  for (int i = 0; i < 2; i++) {
    double a = tests[i][0], b = tests[i][1], c = tests[i][2];
    double r = angle_diff(a, b);
    cout << "a = " << a << ", b = " << b << ": expected = " << c << ", computed = " << r << endl;
  }
}
