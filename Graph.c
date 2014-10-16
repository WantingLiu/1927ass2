// Graph.c ... implementation of Graph ADT
// Written by John Shepherd, May 2013

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "Graph.h"


// graph representation (adjacency matrix)
typedef struct GraphRep {
	int    nV;    // #vertices
	int    nE;    // #edges
	int  **edges; // matrix of weights (0 == no edge)
} GraphRep;


int getGraphElement (Graph g, int v, int w)
{
	assert (g!=NULL);
	return g->edges[v][w];
}

int getnV (Graph g)
{
	assert (g!=NULL);
	return g->nV;
}

// check validity of Vertex
int validV(Graph g, Vertex v)
{
	return (g != NULL && v >= 0 && v < g->nV);
}

// insert an Edge
void insertEdge(Graph g, LocationID v, LocationID w, int wt)
{
	assert(g != NULL && validV(g,v) && validV(g,w));
	g->edges[v][w] += wt;
	g->edges[w][v] += wt;
	g->nE++;
}

// remove an Edge
// - unsets (v,w) and (w,v)
void removeEdge(Graph g, Vertex v, Vertex w)
{
	assert(g != NULL && validV(g,v) && validV(g,w));
	if (g->edges[v][w] != 0) {
		g->edges[v][w] = 0;
		g->edges[w][v] = 0;
		g->nE--;
	}
}

// create an empty graph
Graph newGraph(int nV)
{
	assert(nV > 0);
	
	int i = 0;
	int j = 0;
	
	GraphRep *new = malloc(sizeof(GraphRep));
	new->nV = nV;
	new->nE = 0;
	new->edges = malloc(nV*sizeof(int *));
	
	for (i = 0; i < nV; i++) {
		new->edges[i] = malloc(nV*sizeof(int));
	}
	
	for(i=0;i<nV;i++) {
		for(j=0;j<nV;j++) {
			new->edges[i][j] = 0;
		}
	}
	
	return new;
}

// free memory associated with graph
void dropGraph(Graph g)
{
	assert(g != NULL);

	int i;	
	for (i = 0; i < g->nV; i++) {
		free(g->edges[i]);
	}
	
	free(g);
}

// display graph, using names for vertices
void showGraph(Graph g, char **names)
{
	assert(g != NULL);
	printf("#vertices=%d, #edges=%d\n\n",g->nV,g->nE);
	int v, w;
	for (v = 0; v < g->nV; v++) {
		printf("%d %s\n",v,names[v]);
		for (w = 0; w < g->nV; w++) {
			if (g->edges[v][w]) {
				printf("\t%s (%d)\n",names[w],g->edges[v][w]);
			}
		}
		printf("\n");
	}
}

// display graph, using names for vertices
void showGraph2(Graph g)
{
	assert(g != NULL);
	printf("#vertices=%d, #edges=%d\n\n",g->nV,g->nE);
	int v, w;
	for (v=0; v < g->nV; v++) {
		for (w=0;w<g->nV;w++) {
			printf("%d ",g->edges[v][w]);
		}
		printf("\n");
	}
}

// find a path between two vertices using breadth-first traversal
// only allow edges whose weight is less than "max"
int findPathDist(Graph g, Vertex src, Vertex dest)
{
	assert(g != NULL && validV(g,src) && validV(g,dest));
	int tmp_city = src;
	int index = 0;

	int path_distance = 0;
	int visited[NUM_MAP_LOCATIONS] = {0};
	int prev[NUM_MAP_LOCATIONS] = {[0 ... (NUM_MAP_LOCATIONS-1)] = -1};

	Queue cityQ = newQueue();
	QueueJoin(cityQ, src);

	// While Queue is not empty and the tmp_city is not the destination city (e.g. when path to destination city from src is found)
	while (QueueIsEmpty(cityQ) == 0 && tmp_city != dest) {
		tmp_city = QueueLeave(cityQ);

		// Checks through all potential connected cities
		while (index < g->nV) {
			// Checking if the distance between the two cities is less than the specified maximum and the city being checked is not the destination
			if (g->edges[tmp_city][index] > 1 && g->edges[tmp_city][index] > 4 && g->edges[tmp_city][index] >6
						&& g->edges[tmp_city][index] >7 && visited[index] != 1 && tmp_city != dest) {
				QueueJoin(cityQ, index);
				prev[index] = tmp_city;
				visited[index] = 1;

			// If city is found
			} else if (tmp_city == dest) {
				prev[index] = tmp_city;

				// Calculating size of path
				while (index != src) {
					index = prev[index];
					path_distance++;
				}

				break;
			}

			index++;
		}

		index = 0;
	}

	return path_distance;
}



// Add edges to Graph representing map of Europe
void addConnections(Graph g)
{
	//### ROAD Connections ###

	insertEdge(g, ALICANTE, GRANADA, ROAD);
	insertEdge(g, ALICANTE, MADRID, ROAD);
	insertEdge(g, ALICANTE, SARAGOSSA, ROAD);
	insertEdge(g, AMSTERDAM, BRUSSELS, ROAD);
	insertEdge(g, AMSTERDAM, COLOGNE, ROAD);
	insertEdge(g, ATHENS, VALONA, ROAD);
	insertEdge(g, BARCELONA, SARAGOSSA, ROAD);
	insertEdge(g, BARCELONA, TOULOUSE, ROAD);
	insertEdge(g, BARI, NAPLES, ROAD);
	insertEdge(g, BARI, ROME, ROAD);
	insertEdge(g, BELGRADE, BUCHAREST, ROAD);
	insertEdge(g, BELGRADE, KLAUSENBURG, ROAD);
	insertEdge(g, BELGRADE, SARAJEVO, ROAD);
	insertEdge(g, BELGRADE, SOFIA, ROAD);
	insertEdge(g, BELGRADE, ST_JOSEPH_AND_ST_MARYS, ROAD);
	insertEdge(g, BELGRADE, SZEGED, ROAD);
	insertEdge(g, BERLIN, HAMBURG, ROAD);
	insertEdge(g, BERLIN, LEIPZIG, ROAD);
	insertEdge(g, BERLIN, PRAGUE, ROAD);
	insertEdge(g, BORDEAUX, CLERMONT_FERRAND, ROAD);
	insertEdge(g, BORDEAUX, NANTES, ROAD);
	insertEdge(g, BORDEAUX, SARAGOSSA, ROAD);
	insertEdge(g, BORDEAUX, TOULOUSE, ROAD);
	insertEdge(g, BRUSSELS, COLOGNE, ROAD);
	insertEdge(g, BRUSSELS, LE_HAVRE, ROAD);
	insertEdge(g, BRUSSELS, PARIS, ROAD);
	insertEdge(g, BRUSSELS, STRASBOURG, ROAD);
	insertEdge(g, BUCHAREST, CONSTANTA, ROAD);
	insertEdge(g, BUCHAREST, GALATZ, ROAD);
	insertEdge(g, BUCHAREST, KLAUSENBURG, ROAD);
	insertEdge(g, BUCHAREST, SOFIA, ROAD);
	insertEdge(g, BUDAPEST, KLAUSENBURG, ROAD);
	insertEdge(g, BUDAPEST, SZEGED, ROAD);
	insertEdge(g, BUDAPEST, VIENNA, ROAD);
	insertEdge(g, BUDAPEST, ZAGREB, ROAD);
	insertEdge(g, CADIZ, GRANADA, ROAD);
	insertEdge(g, CADIZ, LISBON, ROAD);
	insertEdge(g, CADIZ, MADRID, ROAD);
	insertEdge(g, CASTLE_DRACULA, GALATZ, ROAD);
	insertEdge(g, CASTLE_DRACULA, KLAUSENBURG, ROAD);
	insertEdge(g, CLERMONT_FERRAND, GENEVA, ROAD);
	insertEdge(g, CLERMONT_FERRAND, MARSEILLES, ROAD);
	insertEdge(g, CLERMONT_FERRAND, NANTES, ROAD);
	insertEdge(g, CLERMONT_FERRAND, PARIS, ROAD);
	insertEdge(g, CLERMONT_FERRAND, TOULOUSE, ROAD);
	insertEdge(g, COLOGNE, FRANKFURT, ROAD);
	insertEdge(g, COLOGNE, HAMBURG, ROAD);
	insertEdge(g, COLOGNE, LEIPZIG, ROAD);
	insertEdge(g, COLOGNE, STRASBOURG, ROAD);
	insertEdge(g, CONSTANTA, GALATZ, ROAD);
	insertEdge(g, CONSTANTA, VARNA, ROAD);
	insertEdge(g, DUBLIN, GALWAY, ROAD);
	insertEdge(g, EDINBURGH, MANCHESTER, ROAD);
	insertEdge(g, FLORENCE, GENOA, ROAD);
	insertEdge(g, FLORENCE, ROME, ROAD);
	insertEdge(g, FLORENCE, VENICE, ROAD);
	insertEdge(g, FRANKFURT, LEIPZIG, ROAD);
	insertEdge(g, FRANKFURT, NUREMBURG, ROAD);
	insertEdge(g, FRANKFURT, STRASBOURG, ROAD);
	insertEdge(g, GALATZ, KLAUSENBURG, ROAD);
	insertEdge(g, GENEVA, MARSEILLES, ROAD);
	insertEdge(g, GENEVA, PARIS, ROAD);
	insertEdge(g, GENEVA, STRASBOURG, ROAD);
	insertEdge(g, GENEVA, ZURICH, ROAD);
	insertEdge(g, GENOA, MARSEILLES, ROAD);
	insertEdge(g, GENOA, MILAN, ROAD);
	insertEdge(g, GENOA, VENICE, ROAD);
	insertEdge(g, GRANADA, MADRID, ROAD);
	insertEdge(g, HAMBURG, LEIPZIG, ROAD);
	insertEdge(g, KLAUSENBURG, SZEGED, ROAD);
	insertEdge(g, LEIPZIG, NUREMBURG, ROAD);
	insertEdge(g, LE_HAVRE, NANTES, ROAD);
	insertEdge(g, LE_HAVRE, PARIS, ROAD);
	insertEdge(g, LISBON, MADRID, ROAD);
	insertEdge(g, LISBON, SANTANDER, ROAD);
	insertEdge(g, LIVERPOOL, MANCHESTER, ROAD);
	insertEdge(g, LIVERPOOL, SWANSEA, ROAD);
	insertEdge(g, LONDON, MANCHESTER, ROAD);
	insertEdge(g, LONDON, PLYMOUTH, ROAD);
	insertEdge(g, LONDON, SWANSEA, ROAD);
	insertEdge(g, MADRID, SANTANDER, ROAD);
	insertEdge(g, MADRID, SARAGOSSA, ROAD);
	insertEdge(g, MARSEILLES, MILAN, ROAD);
	insertEdge(g, MARSEILLES, TOULOUSE, ROAD);
	insertEdge(g, MARSEILLES, ZURICH, ROAD);
	insertEdge(g, MILAN, MUNICH, ROAD);
	insertEdge(g, MILAN, VENICE, ROAD);
	insertEdge(g, MILAN, ZURICH, ROAD);
	insertEdge(g, MUNICH, NUREMBURG, ROAD);
	insertEdge(g, MUNICH, STRASBOURG, ROAD);
	insertEdge(g, MUNICH, VENICE, ROAD);
	insertEdge(g, MUNICH, VIENNA, ROAD);
	insertEdge(g, MUNICH, ZAGREB, ROAD);
	insertEdge(g, MUNICH, ZURICH, ROAD);
	insertEdge(g, NANTES, PARIS, ROAD);
	insertEdge(g, NAPLES, ROME, ROAD);
	insertEdge(g, NUREMBURG, PRAGUE, ROAD);
	insertEdge(g, NUREMBURG, STRASBOURG, ROAD);
	insertEdge(g, PARIS, STRASBOURG, ROAD);
	insertEdge(g, PRAGUE, VIENNA, ROAD);
	insertEdge(g, SALONICA, SOFIA, ROAD);
	insertEdge(g, SALONICA, VALONA, ROAD);
	insertEdge(g, SANTANDER, SARAGOSSA, ROAD);
	insertEdge(g, SARAGOSSA, TOULOUSE, ROAD);
	insertEdge(g, SARAJEVO, SOFIA, ROAD);
	insertEdge(g, SARAJEVO, ST_JOSEPH_AND_ST_MARYS, ROAD);
	insertEdge(g, SARAJEVO, VALONA, ROAD);
	insertEdge(g, SARAJEVO, ZAGREB, ROAD);
	insertEdge(g, SOFIA, VALONA, ROAD);
	insertEdge(g, SOFIA, VARNA, ROAD);
	insertEdge(g, STRASBOURG, ZURICH, ROAD);
	insertEdge(g, ST_JOSEPH_AND_ST_MARYS, SZEGED, ROAD);
	insertEdge(g, ST_JOSEPH_AND_ST_MARYS, ZAGREB, ROAD);
	insertEdge(g, SZEGED, ZAGREB, ROAD);
	insertEdge(g, VIENNA, ZAGREB, ROAD);

	//### RAIL Connections ###

	insertEdge(g, ALICANTE, BARCELONA, RAIL);
	insertEdge(g, ALICANTE, MADRID, RAIL);
	insertEdge(g, BARCELONA, SARAGOSSA, RAIL);
	insertEdge(g, BARI, NAPLES, RAIL);
	insertEdge(g, BELGRADE, SOFIA, RAIL);
	insertEdge(g, BELGRADE, SZEGED, RAIL);
	insertEdge(g, BERLIN, HAMBURG, RAIL);
	insertEdge(g, BERLIN, LEIPZIG, RAIL);
	insertEdge(g, BERLIN, PRAGUE, RAIL);
	insertEdge(g, BORDEAUX, PARIS, RAIL);
	insertEdge(g, BORDEAUX, SARAGOSSA, RAIL);
	insertEdge(g, BRUSSELS, COLOGNE, RAIL);
	insertEdge(g, BRUSSELS, PARIS, RAIL);
	insertEdge(g, BUCHAREST, CONSTANTA, RAIL);
	insertEdge(g, BUCHAREST, GALATZ, RAIL);
	insertEdge(g, BUCHAREST, SZEGED, RAIL);
	insertEdge(g, BUDAPEST, SZEGED, RAIL);
	insertEdge(g, BUDAPEST, VIENNA, RAIL);
	insertEdge(g, COLOGNE, FRANKFURT, RAIL);
	insertEdge(g, EDINBURGH, MANCHESTER, RAIL);
	insertEdge(g, FLORENCE, MILAN, RAIL);
	insertEdge(g, FLORENCE, ROME, RAIL);
	insertEdge(g, FRANKFURT, LEIPZIG, RAIL);
	insertEdge(g, FRANKFURT, STRASBOURG, RAIL);
	insertEdge(g, GENEVA, MILAN, RAIL);
	insertEdge(g, GENOA, MILAN, RAIL);
	insertEdge(g, LEIPZIG, NUREMBURG, RAIL);
	insertEdge(g, LE_HAVRE, PARIS, RAIL);
	insertEdge(g, LISBON, MADRID, RAIL);
	insertEdge(g, LIVERPOOL, MANCHESTER, RAIL);
	insertEdge(g, LONDON, MANCHESTER, RAIL);
	insertEdge(g, LONDON, SWANSEA, RAIL);
	insertEdge(g, MADRID, SANTANDER, RAIL);
	insertEdge(g, MADRID, SARAGOSSA, RAIL);
	insertEdge(g, MARSEILLES, PARIS, RAIL);
	insertEdge(g, MILAN, ZURICH, RAIL);
	insertEdge(g, MUNICH, NUREMBURG, RAIL);
	insertEdge(g, NAPLES, ROME, RAIL);
	insertEdge(g, PRAGUE, VIENNA, RAIL);
	insertEdge(g, SALONICA, SOFIA, RAIL);
	insertEdge(g, SOFIA, VARNA, RAIL);
	insertEdge(g, STRASBOURG, ZURICH, RAIL);
	insertEdge(g, VENICE, VIENNA, RAIL);

	//### BOAT_C Connections ###

	insertEdge(g, ADRIATIC_SEA, BARI, BOAT_C);
	insertEdge(g, ADRIATIC_SEA, IONIAN_SEA, BOAT_C);
	insertEdge(g, ADRIATIC_SEA, VENICE, BOAT_C);
	insertEdge(g, ALICANTE, MEDITERRANEAN_SEA, BOAT_C);
	insertEdge(g, AMSTERDAM, NORTH_SEA, BOAT_C);
	insertEdge(g, ATHENS, IONIAN_SEA, BOAT_C);
	insertEdge(g, ATLANTIC_OCEAN, BAY_OF_BISCAY, BOAT_C);
	insertEdge(g, ATLANTIC_OCEAN, CADIZ, BOAT_C);
	insertEdge(g, ATLANTIC_OCEAN, ENGLISH_CHANNEL, BOAT_C);
	insertEdge(g, ATLANTIC_OCEAN, GALWAY, BOAT_C);
	insertEdge(g, ATLANTIC_OCEAN, IRISH_SEA, BOAT_C);
	insertEdge(g, ATLANTIC_OCEAN, LISBON, BOAT_C);
	insertEdge(g, ATLANTIC_OCEAN, MEDITERRANEAN_SEA, BOAT_C);
	insertEdge(g, ATLANTIC_OCEAN, NORTH_SEA, BOAT_C);
	insertEdge(g, BARCELONA, MEDITERRANEAN_SEA, BOAT_C);
	insertEdge(g, BAY_OF_BISCAY, BORDEAUX, BOAT_C);
	insertEdge(g, BAY_OF_BISCAY, NANTES, BOAT_C);
	insertEdge(g, BAY_OF_BISCAY, SANTANDER, BOAT_C);
	insertEdge(g, BLACK_SEA, CONSTANTA, BOAT_C);
	insertEdge(g, BLACK_SEA, IONIAN_SEA, BOAT_C);
	insertEdge(g, BLACK_SEA, VARNA, BOAT_C);
	insertEdge(g, CAGLIARI, MEDITERRANEAN_SEA, BOAT_C);
	insertEdge(g, CAGLIARI, TYRRHENIAN_SEA, BOAT_C);
	insertEdge(g, DUBLIN, IRISH_SEA, BOAT_C);
	insertEdge(g, EDINBURGH, NORTH_SEA, BOAT_C);
	insertEdge(g, ENGLISH_CHANNEL, LE_HAVRE, BOAT_C);
	insertEdge(g, ENGLISH_CHANNEL, LONDON, BOAT_C);
	insertEdge(g, ENGLISH_CHANNEL, NORTH_SEA, BOAT_C);
	insertEdge(g, ENGLISH_CHANNEL, PLYMOUTH, BOAT_C);
	insertEdge(g, GENOA, TYRRHENIAN_SEA, BOAT_C);
	insertEdge(g, HAMBURG, NORTH_SEA, BOAT_C);
	insertEdge(g, IONIAN_SEA, SALONICA, BOAT_C);
	insertEdge(g, IONIAN_SEA, TYRRHENIAN_SEA, BOAT_C);
	insertEdge(g, IONIAN_SEA, VALONA, BOAT_C);
	insertEdge(g, IRISH_SEA, LIVERPOOL, BOAT_C);
	insertEdge(g, IRISH_SEA, SWANSEA, BOAT_C);
	insertEdge(g, MARSEILLES, MEDITERRANEAN_SEA, BOAT_C);
	insertEdge(g, MEDITERRANEAN_SEA, TYRRHENIAN_SEA, BOAT_C);
	insertEdge(g, NAPLES, TYRRHENIAN_SEA, BOAT_C);
	insertEdge(g, ROME, TYRRHENIAN_SEA, BOAT_C);
}







