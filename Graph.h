// Graph.h ... interface to Graph ADT
// Written by John Shepherd, March 2013

#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>

/*
0 == --- == no access
1 == --x == ROAD
2 == -w- == RAIL
3 == -wx == RAIL / ROAD
4 == r-- == BOAT
5 == r-x == BOAT / ROAD
6 == rw- == BOAT / RAIL
7 == rwx == BOAT / RAIL / ROAD
*/

#define BOAT_C 4

// graph representation is hidden


// graph representation (adjacency matrix)
typedef struct GraphRep {
	int    nV;    // #vertices
	int    nE;    // #edges
	int  **edges; // matrix of weights (0 == no edge)
} GraphRep;

typedef struct GraphRep *Graph;

// vertices denoted by integers 0..N-1
typedef int Vertex;
int   validV(Graph,Vertex); //validity check

// edges are pairs of vertices (end-points)
void insertEdge(Graph, Vertex, Vertex, int);
void removeEdge(Graph, Vertex, Vertex);

// operations on graphs
Graph newGraph(int nV);
void dropGraph(Graph);
Graph makeGraph(int, int**);
void showGraph(Graph, char **);
int  findPath(Graph, Vertex, Vertex, int, int *);

void showGraph2(Graph g);

// Functions to add connections
void addConnections(Graph g);
void addRoadConnections(Graph g);
#endif
