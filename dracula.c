// dracula.c
// Implementation of your "Fury of Dracula" Dracula AI

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "Game.h"
#include "DracView.h"
#include "Queue.h"


static int dracRandomMove(DracView gameState);
static int getHunterDistance(DracView gameState);

void decideDraculaMove(DracView gameState)
{
	int i, minimum; // Minimum stores the distance of the closest hunter
	PlayerID closestHunter = PLAYER_LORD_GODALMING;
	minimum = getHunterDistance(gameState, PLAYER_LORD_GODALMING);
	// Getting distance to each hunter to priortise which one to run from
	for (i = 1; i < 4; i++) {
		if (getHunterDistance(gameState, i) < minimum) {
			minimum = getHunterDistance(gameState, i);
			closestHunter = i;
		}
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

// Function to get distance to a hunter from a specific location
static int getHunterDistance(DracView gameState, PlayerID hunter) {
	LocationID hunterLoc = whereIs(gameState, hunter);




	return 0;
}


/*static int dracRandomMove(DracView gameState)
{
	int* numLocations = NULL;
	int* possibleLocs = whereCanIgo(gameState, numLocations, TRUE, FALSE);

	int randIndex = (rand()%(*numLocations-1));
	return possibleLocs[randIndex];
}*/


