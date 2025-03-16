#include <pybind11/pybind11.h>
#include <ghimo/geometry2d.h>

namespace py = pybind11;

PYBIND11_MODULE(ghimo_python, m) {
    m.doc() = "My Python extension module using pybind11";

    m.def("hello", []() {
        return "Hello, world!";
    });

    m.def("angle_diff", ghimo::angle_diff, "Compute the difference between two angles");
}
