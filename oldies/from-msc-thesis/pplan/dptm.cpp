#include <list>
using std::list;
#include <vector>
using std::vector;
#include <map>
using std::map;
#include <algorithm>
using std::find;
#include <fstream>      // FIXME
using std::ofstream;
using std::endl;

#include "utilz.h"
#include "ppmap.h"
#include "dptm.h"
#ifdef __BORLANDC__
#include "uMain.h"
#endif

// FIXME: we should remove every __BORLANDC__ stuff

ofstream d_os; // FIXME

dptm::dptm() : PMap(0),
    ParamLearnFactorWinner(0.05),
    ParamLearnFactorSecondWinner(0.005),
    ParamMaxEdgeAge(50),
    ParamMaxRandomTries(100)           // in randomPositions()
{
#ifdef __BORLANDC_
    d_os.open("dptm-log.txt"); // FIXME
#else
    d_os.open("pathde/logs/dptm-log");
#endif
}

dptm::~dptm()
{
    d_os.close(); // FIXME
}

void dptm::setPPMap(ppmap& PPMap)
{
    PMap = &PPMap;
}

/*
    NEW POSITIONS FUNCTIONS
*/

void dptm::putPosition(const ppmap::position& Pos, bool TemporaryNode)
{
    const int MAX_DIST = (int) (PMap->getMaxX() + PMap->getMaxY() - PMap->getMinX() - PMap->getMinY());
    if (!PMap->posAllowed(Pos)) return;   // defensive programming
    if (!TemporaryNode) {
        // find the best similar node and the second best similar node to this one
        nodes_iterator winner_iterator = Nodes.end();
        nodes_iterator second_winner_iterator = Nodes.end();
        nodes_iterator new_node_inserted_iterator = Nodes.end();
        nodes_iterator it = Nodes.begin();
        double best_distance = MAX_DIST;
        double second_best_distance = MAX_DIST;
        for ( ; it != Nodes.end(); ) {
            if (it->Temporary) ++it; //it = removeNode(it); // remove temporary nodes (defensive programming)
            else {
                if (!it->Temporary && PMap->posSight(Pos, *it)) {
                    // we check only nodes visible from this configuration
                    double distance = PMap->posDist(Pos, *it);
                    if (distance < best_distance) {
                        second_best_distance = best_distance;
                        second_winner_iterator = winner_iterator;
                        best_distance = distance;
                        winner_iterator = it;
                    }
                    else if (distance < second_best_distance) {
                        second_best_distance = distance;
                        second_winner_iterator = it;
                    }
                }
                ++it;
            }
        }

        if (winner_iterator == Nodes.end()) {
            // if there is no winner (and so there isn't a second winner too),
            // i add this node, because it's either the
            // first one, or it isn't visible from any other node
            node new_node = Pos;
            new_node.Clearance = PMap->nearestObstDistance(new_node);
            new_node.Influence = 0;
            Nodes.push_back(new_node);
            // i can't make edges, because it's the first node or it isn't visible
            // from any other node
        }
        else {
            // there's a winner_node, so we update it
            ppmap::position new_position(winner_iterator->X + ParamLearnFactorWinner * (Pos.X - winner_iterator->X),
                                         winner_iterator->Y + ParamLearnFactorWinner * (Pos.Y - winner_iterator->Y));
            bool due_to_scout;
            if (canMoveNode(*winner_iterator, new_position, &due_to_scout)) {
                winner_iterator->X = new_position.X;
                winner_iterator->Y = new_position.Y;
                winner_iterator->Clearance = PMap->nearestObstDistance(*winner_iterator);
                winner_iterator->Influence = 0.9 * winner_iterator->Influence + 0.1 * best_distance;
            }
            else {
                // if we cannot move a scout winner node (but not because it's a scout),
                // we make a new node and try to connect to the winner, but only if the
                // new node cannot see a neighbour of the winner
                bool have_to_add = false;
                if (!due_to_scout) {
                    have_to_add = true;
                    list<node*> neighs = neighbours(*winner_iterator);
                    for (list<node*>::iterator it = neighs.begin(); it != neighs.end(); ++it) {
                        if (PMap->posSight(Pos, **it)) {
                            have_to_add = false;
                            break;
                        }
                    }
                    if (have_to_add) {
                        node new_node = Pos;
                        new_node.Clearance = PMap->nearestObstDistance(new_node);
                        new_node.Influence = 0;
                        Nodes.push_back(new_node);
                        new_node_inserted_iterator = Nodes.end();
                        new_node_inserted_iterator--;
                        if (PMap->posSight(*winner_iterator, *new_node_inserted_iterator) && !edgeExists(*winner_iterator, *new_node_inserted_iterator)) {
                            makeEdgeP(winner_iterator, new_node_inserted_iterator);
                        }
                    }
                }
            }

            if (second_winner_iterator != Nodes.end()) {
                // there's also a second winner
                if (PMap->posSight(*winner_iterator, *second_winner_iterator)) {
                    // if there's a second winner and it's visible from the first
                    // we make a connection
                    edge* eptr = findEdgeP(*winner_iterator, *second_winner_iterator);
                    if (!eptr) eptr = makeEdgeP(winner_iterator, second_winner_iterator);
                    else eptr->Age = 0;     // if the connection already existed
                                            // we update its age
                }
                else {
                    if (new_node_inserted_iterator != Nodes.end()) {
                        // if the two winner nodes cannot see each other or/and
                        // they are too far, and we just added a new node
                        // we can connect the second winner to the new
                        // inserted node (new position)
                        makeEdgeP(new_node_inserted_iterator, second_winner_iterator);
                    }
                    else {
                        // if the two winners cannot see each other, and we
                        // still not have added a new node, we add a node
                        // and so connect indirectly the two winners

                        // the two nodes may appear to be not in sight, but they could have a link
                        // in this case we don't have to connect them via a third node
                        // FIXME, in effetti potrebbe capitare, nel caso che il nuovo nodo non veda il link
                        if (!edgeExists(*winner_iterator, *second_winner_iterator)) {
                            // first we check if this new node isn't redundant
                            list<node*> neighs = neighbours(*winner_iterator);
                            list<node*> neighs2 = neighbours(*second_winner_iterator);
                            // we make a set-intersection of the two neighbourhoods
                            bool have_to_add = true;
                            for (list<node*>::iterator lnit = neighs.begin(); lnit != neighs.end(); ++lnit) {
                                for (list<node*>::iterator lnit2 = neighs2.begin(); lnit2 != neighs2.end(); ++lnit2) {
                                    if (*lnit == *lnit2) {  // the node is in both neighbourhoods
                                        // FIXME è sufficiente che sia inferiore al nodo trovato, non che siano mergeabili
                                        if (canMergeNodes(**lnit, Pos)) {
                                            have_to_add = false;
                                            break;
                                        }
                                    }
                                }
                            }
                            if (have_to_add) {
                                node new_node = node(Pos);
                                new_node.Clearance = PMap->nearestObstDistance(new_node);
                                new_node.Influence = 0;
                                Nodes.push_back(new_node);
                                new_node_inserted_iterator = Nodes.end();
                                new_node_inserted_iterator--;
                                makeEdgeP(winner_iterator, new_node_inserted_iterator);
                                makeEdgeP(second_winner_iterator, new_node_inserted_iterator);
                            }
                        }
                    }
                }
            }

            // for each node of the neighbourhood of the winner, we update its reference
            // vector and refresh the age of the link
            for (nodes_iterator nit = Nodes.begin(); nit != Nodes.end(); ++nit) {
                if (nit == winner_iterator) continue;
                edges_iterator eit = findEdge(*winner_iterator, *nit);
                if (eit != Edges.end()) {
                    ppmap::position new_position(nit->X + ParamLearnFactorSecondWinner * (Pos.X - nit->X),
                                                 nit->Y + ParamLearnFactorSecondWinner * (Pos.Y - nit->Y));
                    // we update the reference vector only if we don't lose any link
                    if (canMoveNode(*nit, new_position)) {
                        nit->X = new_position.X;
                        nit->Y = new_position.Y;
                        nit->Clearance = PMap->nearestObstDistance(*nit);
                    }
                    eit->Age++;
                    if (eit->Age > ParamMaxEdgeAge) {
                        double distance_with_this_edge = PMap->posDist(*(eit->First), *(eit->Second));
                        double best_distance = MAX_DIST;
                        list<node*> neighbours1 = neighbours(*(eit->First));
                        for (list<node*>::iterator lnit = neighbours1.begin(); lnit != neighbours1.end(); ++lnit) {
                            list<node*> neighbours2 = neighbours(**lnit);
                            double dist1 = PMap->posDist(*(eit->First), **lnit);
                            for (list<node*>::iterator lnit2 = neighbours2.begin(); lnit2 != neighbours2.end(); ++lnit2) {
                                if (*lnit2 == &*(eit->Second)) {
                                    double this_distance = dist1 + PMap->posDist(**lnit, **lnit2);
                                    if (this_distance < best_distance) best_distance = this_distance;
                                }
                            }
                        }
                        if (distance_with_this_edge != 0 && best_distance / distance_with_this_edge < 1.2) {    // FIXME (120%?)
                            removeEdge(eit);
                        }
                    }
                }
            }
        }
#ifdef __BORLANDC__
        frmMain->Winner1 = winner_iterator;
        frmMain->Winner2 = second_winner_iterator;
        frmMain->Redundant1 = Nodes.end();
        frmMain->Redundant2 = Nodes.end();
#endif
        // if the winner and the second winner are redundant, merge them
        if (winner_iterator != Nodes.end() && second_winner_iterator != Nodes.end()) {
            if (canMergeNodes(*winner_iterator, *second_winner_iterator)) {
#ifdef __BORLANDC__
                frmMain->Redundant1 = frmMain->Winner1;
                frmMain->Redundant2 = frmMain->Winner2;
#endif
                mergeNodes(*winner_iterator, *second_winner_iterator);
            }
            else if (isInferiorNode(*winner_iterator, *second_winner_iterator)) {
    //            mergeNodes(*winner_iterator, *second_winner_iterator);
//                removeNode(winner_iterator);
            }
            else if (isInferiorNode(*second_winner_iterator, *winner_iterator)) {
  //              mergeNodes(*winner_iterator, *second_winner_iterator);
//                removeNode(second_winner_iterator);
            }
        }
    }
    else {
        // temporary node:
        // we just add the node and connect to visible nodes
        // (this will be a temporary node, i.e. it will be removed soon)
        node new_node(Pos);
        new_node.Influence = 0;
        new_node.Clearance = 0;
        new_node.Temporary = TemporaryNode;
        Nodes.push_back(new_node);
        nodes_iterator it = Nodes.end();
        --it;
        // connect this node to all nodes visible from it
        nodes_iterator it2 = Nodes.begin();
        for ( ; it2 != Nodes.end(); ++it2) {
            if (it != it2 && !edgeExists(*it, *it2) && PMap->posSight(*it, *it2)) {
                makeEdge(it, it2);
            }
        }
#ifdef __BORLANDC__
        frmMain->Winner1 = Nodes.end();
        frmMain->Winner2 = Nodes.end();
        frmMain->Redundant1 = Nodes.end();
        frmMain->Redundant2 = Nodes.end();
#endif
    }

#ifdef __BORLANDC__
    frmMain->InputX = (int) Pos.X;
    frmMain->InputY = (int) Pos.Y;
#endif
}

void dptm::randomPositions(unsigned int Count)
{
    static list<ppmap::position> PositionsQueue;

    for (unsigned int i = 0; i < Count; i++) {
        if (!PositionsQueue.empty() && myrandom(2) == 0) {
            while (!PositionsQueue.empty() && i < Count) {
                putPosition(PositionsQueue.front());
                PositionsQueue.erase(PositionsQueue.begin());
                i++;
            }
        }
        else {
            if (myrandom(2) == 0) {
                ppmap::position new_pos;
                for (unsigned k = 0; k < ParamMaxRandomTries; k++) {
                    new_pos = PMap->randomPosition();
                    if (PMap->posAllowed(new_pos)) break;
                }
                if (!PMap->posAllowed(new_pos)) break;

                putPosition(new_pos);
            }
            else {
                ppmap::position p1, p2;
                int a = myrandom(4);
                int b = myrandom(3);
                if (b >= a) b++;
                if (a == 0 || a == 2) {
                    p1.X = myrandom((int) (PMap->getMaxX() - PMap->getMinX())) + PMap->getMinX();
                    p1.Y = (a == 0 ? PMap->getMinY() : PMap->getMaxY());
                }
                else {
                    p1.Y = myrandom((int) (PMap->getMaxY() - PMap->getMinY())) + PMap->getMinY();
                    p1.X = (a == 1 ? PMap->getMinX() : PMap->getMaxX());
                }
                if (b == 0 || b == 2) {
                    p2.X = myrandom((int) (PMap->getMaxX() - PMap->getMinX())) + PMap->getMinX();
                    p2.Y = (b == 0 ? PMap->getMinY() : PMap->getMaxY());
                }
                else {
                    p2.Y = myrandom((int) (PMap->getMaxY() - PMap->getMinY())) + PMap->getMinY();
                    p2.X = (b == 1 ? PMap->getMinX() : PMap->getMaxX());
                }
                vector<ppmap::position> new_positions = PMap->gruyereTest(p1, p2);
                for (unsigned int i = 0; i < new_positions.size(); i++) {
                    if (i == 0) {
                        putPosition(new_positions[i]);
                    }
                    else {
                        PositionsQueue.push_back(new_positions[i]);
                    }
                }
#ifdef __BORLANDC__
                frmMain->BridgeTest1 = p1;
                frmMain->BridgeTest2 = p2;
#endif
            }
        }
    }

    for (nodes_iterator nit = Nodes.begin(); nit != Nodes.end(); ) {
        if (nit->EdgesPtrs.size() == 1 && nit->Influence > nit->Clearance) {
            double rand_angle = (double) myrandom(100) / 100 * M_PI * 2;
            double rand_radius = (double) myrandom(100) / 100 * (PMap->getMaxX() + PMap->getMaxY());
            ppmap::position new_pos(nit->X + cos(rand_angle) * rand_radius,
                                    nit->Y - sin(rand_angle) * rand_radius);
            if (!PMap->posSight(*nit, new_pos)) {
                ppmap::position better_new_pos = PMap->firstObstEncountered(*nit, new_pos);
                new_pos.X = 0.1 * nit->X + 0.9 * better_new_pos.X;
                new_pos.Y = 0.1 * nit->Y + 0.9 * better_new_pos.Y;
            }
            PositionsQueue.push_back(new_pos);
        }
    	if (!PMap->posAllowed(*nit)) nit = removeNode(nit);
	else ++nit;
    }

    for (edges_iterator eit = Edges.begin(); eit != Edges.end(); ) {
    	if (!PMap->posSight(*(eit->First), *(eit->Second))) {
		eit = removeEdge(eit);
	}
	else ++eit;
    }
}

/*
    NODES AND EDGES FUNCTIONS
*/

bool dptm::edgeExists(const node& Node1, const node& Node2)
{
    return (findEdgeP(Node1, Node2) != 0);
}

dptm::edges_iterator dptm::removeEdge(const edges_iterator& EdgeIterator)
{
    nodes_iterator nit = EdgeIterator->First;
    nit->EdgesPtrs.remove(&*EdgeIterator);
    nit = EdgeIterator->Second;
    nit->EdgesPtrs.remove(&*EdgeIterator);
    return Edges.erase(EdgeIterator);
}

dptm::edges_iterator dptm::makeEdge(const nodes_iterator& Node1Iterator, const nodes_iterator& Node2Iterator)
{
    Edges.push_back(edge(Node1Iterator, Node2Iterator));
    edges_iterator retval = --Edges.end();
    Node1Iterator->EdgesPtrs.push_back(&*retval);
    Node2Iterator->EdgesPtrs.push_back(&*retval);
    return retval;
}

dptm::edge* dptm::makeEdgeP(const nodes_iterator& Node1Iterator, const nodes_iterator& Node2Iterator)
{
    return &*makeEdge(Node1Iterator, Node2Iterator);
}

dptm::edges_iterator dptm::findEdge(const node& Node1, const node& Node2)
{
    edges_iterator it = Edges.begin();
    for ( ; it != Edges.end(); ++it) {
        if ((&Node1 == &*(it->First) && &Node2 == &*(it->Second)) ||
        (&Node1 == &*(it->Second) && &Node2 == &*(it->First))) {        
            return it;
        }
    }
    return it;
}

dptm::edge* dptm::findEdgeP(const node& Node1, const node& Node2)
{
    list<edge*>::const_iterator leit = Node1.EdgesPtrs.begin();
    for ( ; leit != Node1.EdgesPtrs.end(); ++leit) {
        if ((&*((*leit)->First) == &Node1 && &*((*leit)->Second) == &Node2) ||
        (&*((*leit)->First) == &Node2 && &*((*leit)->Second) == &Node1)) {
            return *leit;
        }
    }
    return 0;
}

dptm::nodes_iterator dptm::removeNode(const dptm::nodes_iterator& NodeIterator)
{
    // delete edges with this node
    for (edges_iterator it = Edges.begin(); it != Edges.end(); ) {
        if (it->First == NodeIterator || it->Second == NodeIterator) it = removeEdge(it);
        else ++it;
    }
    // delete the node
    return Nodes.erase(NodeIterator);
}

void dptm::removeTemporaryNodes()
{
    for (nodes_iterator nit = Nodes.begin(); nit != Nodes.end(); ) {
        if (nit->Temporary) nit = removeNode(nit);
        else ++nit;
    }
}

void dptm::mergeNodes(node& Node1, node& Node2)
{
    if (Node1.EdgesPtrs.empty()) return;    // defensive programming
    if (Node2.EdgesPtrs.empty()) return;    // defensive programming
    nodes_iterator node1_iterator, node2_iterator;

    list<edge*>::const_iterator leit = Node1.EdgesPtrs.begin();
    if (&*(*leit)->First == &Node1) node1_iterator = (*leit)->First;
    else node1_iterator = (*leit)->Second;

    leit = Node2.EdgesPtrs.begin();
    if (&*(*leit)->First == &Node2) node2_iterator = (*leit)->First;
    else node2_iterator = (*leit)->Second;

    for (list<edge*>::const_iterator leit = Node2.EdgesPtrs.begin(); leit != Node2.EdgesPtrs.end(); ++leit) {
        nodes_iterator neighbour;
        if (&*(*leit)->First == &Node2) neighbour = (*leit)->Second;
        else neighbour = (*leit)->First;
        if (&*neighbour == &Node1) continue;
        if (!edgeExists(Node1, *neighbour)) {
            makeEdgeP(node1_iterator, neighbour);
        }
    }
    Node1.X = (Node1.X + Node2.X) / 2;
    Node1.Y = (Node1.Y + Node2.Y) / 2;
    Node1.Clearance = PMap->nearestObstDistance(Node1);
    removeNode(node2_iterator);
}

list<dptm::node*> dptm::neighbours(const dptm::node& Node)
{
    list<node*> retval;
    for (list<edge*>::const_iterator epit = Node.EdgesPtrs.begin(); epit != Node.EdgesPtrs.end(); ++epit) {
        node* adjacent_node_ptr;
        if (&*(*epit)->First == &Node) adjacent_node_ptr = &*(*epit)->Second;
        else adjacent_node_ptr = &*(*epit)->First;
        retval.push_back(adjacent_node_ptr);
    }
    return retval;
}

bool dptm::canMergeNodes(const node& Node1, const node& Node2)
{
    // FIXME we have to check if in the new position the edges are allowed
    if (!PMap->posSight(Node1, Node2)) return false;
    list<node*> neighs = neighbours(Node2);
    bool redundant = true;
    for (list<node*>::iterator it = neighs.begin(); it != neighs.end(); ++it) {
        if (!PMap->posSight(Node1, **it)) {
            redundant = false;
            break;
        }
    }
    if (redundant) {
        neighs = neighbours(Node1);
        for (list<node*>::iterator it = neighs.begin(); it != neighs.end(); ++it) {
            if (!PMap->posSight(Node2, **it)) {
                redundant = false;
                break;
            }
        }
    }
    return redundant;
}

bool dptm::isInferiorNode(const node& Node1, const node& Node2)
{
    if (!PMap->posSight(Node1, Node2)) return false;
    list<node*> neighs = neighbours(Node1);
    bool redundant = true;
    for (list<node*>::iterator it = neighs.begin(); it != neighs.end(); ++it) {
        if (!PMap->posSight(Node2, **it)) {
            redundant = false;
            break;
        }
    }
    return redundant;
}

bool dptm::canMoveNode(const node& Node, const ppmap::position& NewPos, bool* DueToScout)
{
    // a node cannot be moved if it has only one link and the movement make that link shorter
    // (scout mode)
    if (Node.EdgesPtrs.size() == 1) {
        list<edge*>::const_iterator epit = Node.EdgesPtrs.begin();
        node* adjacent_node;
        if (&*(*epit)->First == &Node) adjacent_node = &*(*epit)->Second;
        else adjacent_node = &*(*epit)->First;
        double current_edge_length = PMap->posDist(*adjacent_node, Node);
        double new_edge_length = PMap->posDist(*adjacent_node, NewPos);
        if (new_edge_length < current_edge_length) {
            if (DueToScout) *DueToScout = true;
            return false;
        }
    }

    if (DueToScout) *DueToScout = false;

    // a node cannot be moved if the new position isn't allowed
    if (!PMap->posAllowed(NewPos)) return false;

    // a node cannot be moved if we lose any edge
    bool some_edge_disappear = false;
    for (list<edge*>::const_iterator epit = Node.EdgesPtrs.begin(); epit != Node.EdgesPtrs.end(); ++epit) {
        node* adjacent_node;
        if (&*(*epit)->First == &Node) adjacent_node = &*(*epit)->Second;
        else adjacent_node = &*(*epit)->First;
        if (!PMap->posSight(ppmap::position(adjacent_node->X, adjacent_node->Y), NewPos)) {
            some_edge_disappear = true;
            break;
        }
    }
    if (some_edge_disappear) return false;

    return true;
}

/*
    TOPOLOGICAL PATH
*/

list<ppmap::position> dptm::findTopologicalPath(const ppmap::position& StartPosition,
    const ppmap::position& TargetPosition, bool NewPositions)	// NewPositions = true
{
    const unsigned int MAX_DIST = (int) (PMap->getMaxX() + PMap->getMaxY() - PMap->getMinX() - PMap->getMinY());
    const unsigned int MAX_ROUTE = MAX_DIST * MAX_DIST;

    list<ppmap::position> retval;

    if (!PMap->posAllowed(StartPosition) || !PMap->posAllowed(TargetPosition)) {
        retval.push_back(StartPosition);
        return retval;
    }

    if (NewPositions) {
        removeTemporaryNodes();
        putPosition(StartPosition, true);
        putPosition(TargetPosition, true);
    }

    // find the node nearest to the start node
    nodes_iterator it = Nodes.begin();
    double best_distance = PMap->posDist(StartPosition, *it);
    nodes_iterator first_node_it = Nodes.begin();
    while (it != Nodes.end()) {
        double distance = PMap->posDist(StartPosition, *it);
        if (distance < best_distance && PMap->posSight(StartPosition, *it)) {
            first_node_it = it;
            best_distance = distance;
        }
        ++it;
    }

    // find the node nearest to the target configuration
    it = Nodes.begin();
    best_distance = PMap->posDist(TargetPosition, *it);
    nodes_iterator last_node_it = Nodes.begin();
    while (it != Nodes.end()) {
        double distance = PMap->posDist(TargetPosition, *it);
        if (distance < best_distance && PMap->posSight(TargetPosition, *it)) {
            last_node_it = it;
            best_distance = distance;
        }
        ++it;
    }

    // DIJKSTRA ALGORITHM

    // initialization of the unknown nodes pointers and the cost and prev maps
    vector<node*> unknown_nodes_ptrs;
    map<node*, double> cost;
    map<node*, node*> prev;
    for (nodes_iterator nit = Nodes.begin(); nit != Nodes.end(); ++nit) {
        if (first_node_it != nit) unknown_nodes_ptrs.push_back(&*nit);
        edge* eptr = findEdgeP(*first_node_it, *nit);
        if (eptr && !eptr->Unsafe) {
            cost[&*nit] = PMap->posDist(*first_node_it, *nit);
            prev[&*nit] = &*first_node_it;
        }
        else {
            cost[&*nit] = MAX_ROUTE; // FIXME (MAX DOUBLE)
            prev[&*nit] = 0;
        }
    }

    if (unknown_nodes_ptrs.size() == 0) {
        retval.push_back(StartPosition);
        return retval;
    }

    // cycle
    do {
        // find the unknown node with minimum cost
        node* min_node_ptr = unknown_nodes_ptrs[0];
        for (unsigned int i = 0; i < unknown_nodes_ptrs.size(); i++) {
            if (cost[unknown_nodes_ptrs[i]] < cost[min_node_ptr]) min_node_ptr = unknown_nodes_ptrs[i];
        }
        // delete it from unknown nodes
        unknown_nodes_ptrs.erase(find(unknown_nodes_ptrs.begin(), unknown_nodes_ptrs.end(), min_node_ptr));
        // for each node adjacent to this minimum node
        for (list<edge*>::iterator epit = min_node_ptr->EdgesPtrs.begin(); epit != min_node_ptr->EdgesPtrs.end(); ++epit) {
            if ((*epit)->Unsafe) continue;
            // there's an edge, so "First" or "Second" is a node adjacent to the minimim node
            node* node_to_update_ptr;
            if (&*(*epit)->First == min_node_ptr) node_to_update_ptr = &*(*epit)->Second;
            else node_to_update_ptr = &*(*epit)->First;
            // if this adjacent node is an unknown node, update it
            if (find(unknown_nodes_ptrs.begin(), unknown_nodes_ptrs.end(), node_to_update_ptr) != unknown_nodes_ptrs.end()) {
                double newcost = cost[min_node_ptr] + PMap->posDist(*min_node_ptr, *node_to_update_ptr);
                if (newcost < cost[node_to_update_ptr]) {
                    cost[node_to_update_ptr] = newcost;
                    prev[node_to_update_ptr] = min_node_ptr;
                }
            }
        }
    } while (!unknown_nodes_ptrs.empty());

    node* path_node_ptr = &*last_node_it;
#ifdef __BORLANDC__
    frmMain->PathCost = cost[path_node_ptr];  // FIXME
#endif
    if (cost[path_node_ptr] >= MAX_ROUTE) { 
        retval.push_back(StartPosition);
    }
    else {
        // start and target configurations are INSIDE the network
        // (they are temporary nodes)
        // so we don't have to add them manually in the path
        do {
            retval.push_front(*path_node_ptr);
            path_node_ptr = prev[path_node_ptr];
        } while (path_node_ptr != &*first_node_it);

        retval.push_front(*first_node_it);
    }

    return retval;
}

/*
    OPTIMIZING FUNCTIONS
*/

void dptm::removeCrosses()
{
    // FIXME: terribly unefficient!!
    bool again;
    do {
        again = false;
        for (edges_iterator eit = Edges.begin(); eit != Edges.end(); ++eit) {
            for (edges_iterator eit2 = Edges.begin(); eit2 != Edges.end(); ++eit2) {
                if (&*eit == &*eit2) continue;
                if (&*eit->First == &*eit2->First || &*eit->First == &*eit2->Second
                || &*eit->Second == &*eit2->First || &*eit->Second == &*eit2->Second) continue;
                if (PMap->posDist(*eit->First, *eit->Second) <= PMap->ParamEpsilon) continue;
                if (PMap->posDist(*eit2->First, *eit2->Second) <= PMap->ParamEpsilon) continue;
                if (PMap->posDist(*eit->First, *eit2->First) <= PMap->ParamEpsilon) continue;
                if (PMap->posDist(*eit->Second, *eit2->Second) <= PMap->ParamEpsilon) continue;
                if (PMap->posDist(*eit->First, *eit2->Second) <= PMap->ParamEpsilon) continue;
                if (PMap->posDist(*eit2->First, *eit->Second) <= PMap->ParamEpsilon) continue;
                if (PMap->sedgewickIntersect(eit->First->X, eit->First->Y, eit->Second->X, eit->Second->Y,
                                              eit2->First->X, eit2->First->Y, eit2->Second->X, eit2->Second->Y))
                {
                    ppmap::position p = PMap->segmentsIntersection(*eit->First, *eit->Second, *eit2->First, *eit2->Second);
                    if (p.X == 0 && p.Y == 0) continue;
                    node n(p);
                    Nodes.push_back(n);
                    nodes_iterator nit = Nodes.end();
                    --nit;
                    makeEdgeP(eit->First, nit);
                    makeEdgeP(eit->Second, nit);
                    makeEdgeP(eit2->First, nit);
                    makeEdgeP(eit2->Second, nit);
                    removeEdge(eit);
                    removeEdge(eit2);
                    again = true;
                    break;
                }
            }
            if (again) break;
        }
    } while (again);
}

void dptm::removeRedundantEdges()
{
    double MAX_DIST = PMap->getMaxX() + PMap->getMaxY() - PMap->getMinX() - PMap->getMinY();
    for (edges_iterator eit = Edges.begin(); eit != Edges.end(); ) {
        node& node1 = *eit->First;
        node& node2 = *eit->Second;
        ppmap::position middlepos((node1.X + node2.X) / 2, (node1.Y + node2.Y) / 2);
        double min_distance = MAX_DIST;
        node* nearest_node_ptr = 0;
        for (nodes_iterator nit = Nodes.begin(); nit != Nodes.end(); ++nit) {
            double distance = PMap->posDist(*nit, middlepos);
            if (PMap->posSight(*nit, middlepos) && distance < min_distance) {
                nearest_node_ptr = &*nit;
                min_distance = distance;
            }
        }
        if (nearest_node_ptr && (nearest_node_ptr != &node1 && nearest_node_ptr != &node2)) {
            eit = removeEdge(eit);
        }
        else ++eit;
    }
}

