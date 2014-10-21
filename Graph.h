// Graph.h ... interface to Graph ADT
// Written by John Shepherd, March 2013

#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include "Item.h"
#include "Queue.h"
#include "Places.h"

/*
key for graph
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

typedef struct GraphRep *Graph;
typedef int Vertex;

int getGraphElement (Graph g, int v, int x);
int getnV (Graph g);
int validV(Graph,Vertex); //validity check
void insertEdge(Graph, Vertex, Vertex, int);
void removeEdge(Graph, Vertex, Vertex);
Graph newGraph(int nV);
void dropGraph(Graph);
Graph makeGraph(int, int**);
void showGraph(Graph, char **);
int findPathDist(Graph g, Vertex src, Vertex dest);
int findPathGraph(Graph g, Vertex src, Vertex dest, int max, int *path);

void showGraph2(Graph g);

// Functions to add connections
void addConnections(Graph g);
void addRoadConnections(Graph g);
#endif
