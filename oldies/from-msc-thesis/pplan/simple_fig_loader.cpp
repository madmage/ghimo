#include <vector>
using std::vector;
#include <cstdio>
using std::sscanf;
#include <fstream>
using std::ifstream;
using std::ofstream;
using std::endl;
#include <string>
using std::string;

#include "simple_fig_loader.h"

string my_getline(ifstream& ifs)
{
    if (ifs.eof()) return "";
    char buffer[2048];
    while (!ifs.eof()) {
        ifs.getline(buffer, 1024);
        if (buffer[0] != '#') break;
    }
    if (ifs.eof()) return "";
    else return (string) buffer;
}

bool simple_fig_loader::loadFig(string FileName)
{
    ifstream ifs;
    ifs.open(FileName.c_str());

    if (!ifs.is_open()) return false;

    string myline;

    for (int i = 0; i < 8; i++) {
        // reads the first eight lines (options)
        my_getline(ifs);
    }

    while (!ifs.eof()) {
        myline = my_getline(ifs);
        int id;
        if (sscanf(myline.c_str(), "%d", &id) == 0) break;;
        switch (id) {
        case 2: // POLYLINE
        {
            int thickness, npoints;
            sscanf(myline.c_str(),
        		"%*d %*d %*d %d %*d %*d %*d %*d %*d %*f %*d %*d %*d %*d %*d %d",
                &thickness,
                &npoints);

        	int n = 0, index = 0, read = 0;
            int x1, y1, x2, y2;
            bool first = true;
            segment seg;
            seg.Thickness = thickness;
        	myline = my_getline(ifs);
        	while (n < npoints - 1) {
        		if (2 > sscanf(myline.c_str() + (index += read), "%d %d%n", &(x2), &(y2), &read)) {
                    myline = my_getline(ifs);
                    if (myline == "") break;
                    index = read = 0;
                }
                else {
                    if (!first) {
                        seg.X1 = x1;
                        seg.Y1 = y1;
                        seg.X2 = x2;
                        seg.Y2 = y2;
        		    	Segments.push_back(seg);
	        		    n++;
                    }
                    else first = false;
                    x1 = x2;
                    y1 = y2;
                }
            }
            }
		}
	}

    ifs.close();
    return true;
}
