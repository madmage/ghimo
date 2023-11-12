#ifndef dptmH
#define dptmH

#include <list>

#include "ppmap.h"

class dptm {
private:
    ppmap* PMap;
public:
    // structors
    dptm();
    ~dptm();

    // subclasses 1 of 2
    class edge;

    class node : public ppmap::position {
    public:
        // structors
        node() :
            ppmap::position(0, 0), Influence(0), Clearance(0), Temporary(false) {}
        node(ppmap::position Pos) :
            ppmap::position(Pos), Influence(0), Clearance(0), Temporary(false) {}

        // members
        double Clearance;
        double Influence;
        bool Temporary;
        std::list<edge*> EdgesPtrs;
    };

    // typedefs 1 of 2
    typedef std::list<node>::iterator nodes_iterator;

    // subclasses 2 of 2
    class edge {
    public:
        edge(nodes_iterator First, nodes_iterator Second) : First(First), Second(Second), Age(0), Unsafe(false) {}
        edge(nodes_iterator First, nodes_iterator Second, int Age) : First(First), Second(Second), Age(Age), Unsafe(false) {}
        nodes_iterator First;
        nodes_iterator Second;
        unsigned int Age;
		bool Unsafe;
    };

    // typedefs 2 of 2
    typedef std::list<edge>::iterator edges_iterator;

    // members
    std::list<node> Nodes;
    std::list<edge> Edges;

    // parameters
    double ParamLearnFactorWinner;
    double ParamLearnFactorSecondWinner;
    unsigned int ParamMaxEdgeAge;
    unsigned int ParamMaxRandomTries;

    // general functions
    void             setPPMap(ppmap& PPMap);
    void             putPosition(const ppmap::position& Pos, bool TemporaryNode = false);
    void             randomPositions(unsigned int Count);

    // nodes and edges functions
    bool             edgeExists(const node& Node1, const node& Node2);
    edges_iterator   findEdge(const node& Node1, const node& Node2);
    edge*            findEdgeP(const node& Node1, const node& Node2);
    nodes_iterator   removeNode(const nodes_iterator& NodeIterator);
    edges_iterator   removeEdge(const edges_iterator& EdgeIterator);
    edges_iterator   makeEdge(const nodes_iterator& Node1Iterator, const nodes_iterator& Node2Iterator);
    edge*            makeEdgeP(const nodes_iterator& Node1Iterator, const nodes_iterator& Node2Iterator);
    std::list<node*> neighbours(const node& Node);
    bool             canMergeNodes(const node& Node1, const node& Node2);
    bool             isInferiorNode(const node& Node1, const node& Node2);
    void             mergeNodes(node& Node1, node& Node2);
    bool             canMoveNode(const node& Node, const ppmap::position& NewPos, bool* DueToScout = 0);

    void             removeTemporaryNodes();
    void             removeCrosses();
    void             removeRedundantEdges();

    // topological path
    std::list<ppmap::position>
        findTopologicalPath(const ppmap::position& StartPosition, const ppmap::position& TargetPosition, bool NewPositions = true);
};

#endif
