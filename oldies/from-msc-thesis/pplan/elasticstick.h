#ifndef elasticstick_pathplannerH
#define elasticstick_pathplannerH

#include <list>

#include "ppmap.h"

enum es_result {ES_NONE, ES_SAFE, ES_UNSAFE, ES_BROKEN};

class elasticstick {
private:
    ppmap* PMap;
public:
    // structors
    elasticstick();
    ~elasticstick();

    // subclasses
    class node : public ppmap::configuration {
    public:
        double BubbleRadius;
    };

    // functions
    void setPPMap(ppmap& PPMap);
    es_result initialize(const std::list<ppmap::position>& TopologicalPath);
    es_result adjust(unsigned int IterationsCount);

    // members
    std::list<node> Stick;
    node* CriticalBubblePtr;

    // parameters
    double ParamMinObstDist;
    double ParamMinSteeringRadius;
};

#endif
