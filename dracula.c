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

static int dracRandomMove(DracView gameState);
static int getHunterDistance(DracView gameState);

void decideDraculaMove(DracView gameState)
{
	LocationID bestPlay;
	Graph gameMap = newGraph(NUM_MAP_LOCATIONS);
	addConnections(gameMap);
	int path[NUM_MAP_LOCATIONS];

	int i, minimum, tempMinimum; // Minimum stores the distance of the closest hunter
	PlayerID closestHunter = PLAYER_LORD_GODALMING;
	minimum = findPath(gameMap, whereIs(PLAYER_DRACULA), whereIs(PLAYER_LORD_GODALMING), MAX_EDGE_WEIGHT, path); // Defaulting to PLAYER_LORD_GODALMING initially

	// Getting distance to each hunter to priortise which one to run from
	for (i = 1; i < 4; i++) {
		tempMinimum = findPath(gameMap, whereIs(PLAYER_DRACULA), whereIs(i), MAX_EDGE_WEIGHT, path);
		if (tempMinimum < minimum) {
			minimum = tempMinimum;
			closestHunter = i;
		}
	}

	// Getting locations Dracula can go





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


