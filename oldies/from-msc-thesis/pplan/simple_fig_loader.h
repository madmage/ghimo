#ifndef simple_fig_loaderH
#define simple_fig_loaderH

#include <string>
#include <vector>

class segment {
public:
    double X1, Y1, X2, Y2;
    double Thickness;
};

class simple_fig_loader {
public:
    bool loadFig(std::string FileName);
    std::vector<segment> Segments;
};

#endif
