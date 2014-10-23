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

static char* convertTrail(int trail[TRAIL_SIZE], int bestPlay);
static int inTrail(int trail[TRAIL_SIZE], int bestPlay);


void decideDraculaMove(DracView gameState)
{
	printf("12345\n");
	LocationID bestPlay = CASTLE_DRACULA;
	// Getting locations Dracula can go to

	if (giveMeTheRound(gameState) == 0) {
		bestPlay = ATHENS;
	} else {
		int numLocations;
		int *paths = whereCanIgo(gameState, &numLocations, TRUE, FALSE);

		if (numLocations > 0) {
			Graph gameMap = newGraph(NUM_MAP_LOCATIONS);
		   // Only dealing with road connections for now
	//Bernice comments: I wonder if we can just get the map by gameState->view->map (where 'map' is a Graph value in GameView.c and 'view' is a gameView value in HunterView.c)
	//   if this was possible, we may not have to generate and populate a local graph everytime
			addRoadConnections(gameMap);

			int i, minimum, tempMinimum; // Minimum stores the distance of the closest hunter
			PlayerID closestHunter = PLAYER_LORD_GODALMING;

			minimum = findPathDist(gameMap, whereIs(gameState, PLAYER_DRACULA), whereIs(gameState, PLAYER_LORD_GODALMING)); // Defaulting to PLAYER_LORD_GODALMING initially
		  // Getting distance to each hunter to priortise which one to run from
	//Bernice comments: rather have a global variable NUM_PLAYERS instead of a magic number      
			for (i = 1; i < NUM_PLAYERS-1; i++) {
		//Bernice comments: Brady has implemented a findPathDist() function that only returns the distance without having to pass in an array pointer.
		//    rather use findPathDist() to eliminate any possibility of seg faults because we know how annoying C is when this happens
				tempMinimum = findPathDist(gameMap, whereIs(gameState, PLAYER_DRACULA), whereIs(gameState, i));
				if (tempMinimum < minimum) {
					minimum = tempMinimum;
					closestHunter = i;
				}
			}

			  // Finding location that is furthest away from the closest hunter
			bestPlay = paths[0];
			minimum = findPathDist(gameMap, closestHunter, paths[0]);
			for (i = 1; i < numLocations; i++) {
				tempMinimum = findPathDist(gameMap, closestHunter, paths[i]);
				if (tempMinimum < minimum) {
					minimum = tempMinimum;
					bestPlay = paths[i];
				}
			}
			// printf("bestPlay:%s\n", idToAbbrev(bestPlay));
		} else if (numLocations == 0) {
			bestPlay = CASTLE_DRACULA;
		}
	}
	printf("54321\n");


	// Converting to double backs and hide if needed
	int trail[TRAIL_SIZE];
	giveMeTheTrail(gameState, PLAYER_DRACULA, trail);
	

	if (inTrail(trail, bestPlay)) {
		printf("Here\n");
		registerBestPlay(convertTrail(trail, bestPlay), "");
	} else {
		printf("Here2\n");
		registerBestPlay(idToAbbrev(bestPlay), "");
	}
   
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


static int inTrail(int trail[TRAIL_SIZE], int bestPlay) {
	int i;
	int isIn = 0;
	// Doesn't include last move on the trail, can go there because it will drop off
	for (i = 0; i < TRAIL_SIZE-1; i++) {
		if (bestPlay == trail[i]) {
			isIn = 1;
		}	
	}
	return isIn;
}

static char* convertTrail(int trail[TRAIL_SIZE], int bestPlay) {
	int i;
	int hasHide = 0;
	int hasDouble = 0;
	int howFarBack = 0;
	char* converted = "";


	// Checking if there is a hide or double back in the trail, ignoring last element as can go there without double back and hide
	for (i = 0; i < TRAIL_SIZE-1; i++) {
		if (trail[i] == HIDE) {
			hasHide = 1;
		} else if (trail[i] > 102 && trail[i] < 108) {
			hasDouble = 1;
		}
	}

	for (i = 0; i < TRAIL_SIZE-1; i++) {
		if (bestPlay == trail[i]) {
			howFarBack = i;
			break;
		}
	}

	// if there is a double back in the first place of the trail, if there is a double back means there is no hide
	// If double back is first move, hide can potentially be used 

	// If there is a double back in trail and the bestPlay is also in the trail, has to mean hide
	if (hasDouble) {
		converted = "HI";
	} else if (hasHide) {
		switch (howFarBack) {
			case 0:
				converted = "D1";
				break;
			case 1:
				converted = "D2";
				break;
			case 2:
				converted = "D3";
				break;
			case 3:
				converted = "D4";
				break;
			case 4:
				converted = "D5";
				break;
		}
	} else {
		// If there is neither hide or double back in trail
		if (trail[0] == bestPlay) {
			converted = "HI";
		} else {
			switch (howFarBack) {
				case 0:
					converted = "D1";
					break;
				case 1:
					converted = "D2";
					break;
				case 2:
					converted = "D3";
					break;
				case 3:
					converted = "D4";
					break;
				case 4:
					converted = "D5";
					break;
			}
		}
	}

	return converted;
}


/*static int dracRandomMove(DracView gameState)
{
   int* numLocations = NULL;
   int* possibleLocs = whereCanIgo(gameState, numLocations, TRUE, FALSE);

   int randIndex = (rand()%(*numLocations-1));
   return possibleLocs[randIndex];
}
*/

