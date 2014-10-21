// dracula.c
// Implementation of your "Fury of Dracula" Dracula AI

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "Game.h"
#include "DracView.h"
#include "Graph.h"
#include "Places.h"

#define MAX_EDGE_WEIGHT 100

void decideDraculaMove(DracView gameState)
{
	LocationID bestPlay;
	// Getting locations Dracula can go to
	int numLocations;
	int *paths = whereCanIgo(gameState, &numLocations, TRUE, FALSE);

	// If Dracula has at least 1 legal move
	if (numLocations > 0) {
		Graph gameMap = newGraph(NUM_MAP_LOCATIONS);
		// Only dealing with road connections for now
		addRoadConnections(gameMap);
		int path[NUM_MAP_LOCATIONS];

		int i, minimum, tempMinimum; // Minimum stores the distance of the closest hunter
		PlayerID closestHunter = PLAYER_LORD_GODALMING;
		minimum = findPathGraph(gameMap, whereIs(gameState, PLAYER_DRACULA), whereIs(gameState, PLAYER_LORD_GODALMING), MAX_EDGE_WEIGHT, path); // Defaulting to PLAYER_LORD_GODALMING initially

		// Getting distance to each hunter to priortise which one to run from
		for (i = 1; i < 4; i++) {
			tempMinimum = findPathGraph(gameMap, whereIs(gameState, PLAYER_DRACULA), whereIs(gameState, i), MAX_EDGE_WEIGHT, path);
			if (tempMinimum < minimum) {
				minimum = tempMinimum;
				closestHunter = i;
			}
		}

		// Finding location that is furthest away from the closest hunter
		bestPlay = paths[0];
		minimum = findPathGraph(gameMap, closestHunter, paths[0], MAX_EDGE_WEIGHT, path);
		for (i = 1; i < numLocations; i++) {
			tempMinimum = findPathGraph(gameMap, closestHunter, paths[i], MAX_EDGE_WEIGHT, path);
			if (tempMinimum < minimum) {
				minimum = tempMinimum;
				bestPlay = paths[i];
			}
		}
	} else {

		bestPlay = CASTLE_DRACULA;

	}


	registerBestPlay(idToName(bestPlay), "");
	
	/*
	
	Random Move Immediately: Everything
		remeber to not make illegal moves
		Not ocean
		
	Don't move within 1 of a hunter
	
	If below 10 life, move towards castle
	Rail links?
	\
	
	Weighted Moves:
	
	Away from hunters
	If below 10 life, move towards castle
	When to ocen
	Look for bottlenecks and make traps
	
	*/
}

/*static int dracRandomMove(DracView gameState)
{
	int* numLocations = NULL;
	int* possibleLocs = whereCanIgo(gameState, numLocations, TRUE, FALSE);

	int randIndex = (rand()%(*numLocations-1));
	return possibleLocs[randIndex];
}*/


