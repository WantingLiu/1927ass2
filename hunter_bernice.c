// hunter.c
// Implementation of your "Fury of Dracula" hunter AI

#include <stdlib.h>
#include <stdio.h>
#include "Game.h"
#include "HunterView.h"

void decideHunterMove(HunterView gameState)
{
    // TODO ...
    // Replace the line below by something better
    registerBestPlay("GE","I'm on holiday in Geneva");
}


//-===================================================-//

/*
Functions to return the next best LocationID for the currplayer.

-bestLeaderMove() is the main function to return best next move.

-whoIsClosestToCastle() returns the PlayerID who is closest to CASTLE_DRACULA
-bestMoveToCast() returns the best possible move that's closest to CASTLE_DRACULA
-bestExplodeMove() calculates and returns the best possible move that's furthest from all other hunters
hunter.c
*/

// EXPLODE TACIC
// returns next possible move that's furthest from all players
// determination (ranking) of the location is from
// the summation of  distance from all other players to that possible location
static LocationID bestExplodeMove (HunterView g) 
{
   PlayerID currPlayer = WhoAmI(g);

   int *numLocations;
   LocationID *possibleMove = whereCanIGo(g, numLocations, 1, 1, 1 );
   //initially set to player's current location
   LocationID furthestMove = possibleMove[0];
   int maxDist = 0;

   int i;
   for (i=0; i<numLocations; i++) {
      LocationID moveOption = possibleMove[i];
      int sumDist = 0;
      PlayerID p;
      for (p=0; p<(NUM_PLAYERS-1); p++) {
         if (p!=currPlayer) {
            sumDist += findPathDist(g, moveOption, whereIs(g, p) );
         }
      }
   }

   if (sumDist > maxDist) {
      furthestMove = moveOption;
   }

   return furthestMove;
}


//returns the playerID of the player whose position is closest to CASTLE_DRACULA
static PlayerID whoIsClosestToCastle (HunterView g) 
{
   PlayerID closestPlayer = 0;
   int minDist = findPathDist(g, whereIs(g,closestPlayer) );

   PlayerID p;
   for (p=1; p<(NUM_PLAYERS-1); p++) {
      int distToCast = findPathDist(g, whereIs(g,p) );
      if (distToCast < minDist) {
         closestPlayer = p;
      }
   }

   return closestPlayer;
}


//returns the next possible move closest to CASTLE_DRACULA
static LocationID bestMoveToCast (HunterView g, PlayerID player)
{
   int *numLocations;
   LocationID *possibleMove = whereCanTheyGo(g, numLocations, player, 1, 1, 1);

   LocationID closestToCast = possibleMove[0];
   int minDistToCast = findPathDist(g, closestToCast, CASTLE_DRACULA);

   int i;
   for (i=1; i<numLocations; i++) {
      LocationID checkMove = possibleMove[i];
      int distToCast = findPathDist(g, checkMove, CASTLE_DRACULA);

      if (distToCast < minDistToCast) {
         closestToCast = checkMove;
      }
   }

   return closestToCast;
}


////
//main function to decide player's next best move//
static LocationID bestLeaderMove (HunterView g) 
{
   PlayerID currPlayer = WhoAmI(g);

   LocationID nextBestMove;
   //if player happens to be closest to CASTLE_DRACULA, move closer
   if (whoIsClosestToCastle(g) == currPlayer) {
      nextBestMove = bestMoveToCast(g, currPlayer);
   }
   //else move to location furthest to every other player
   //aka: EXPLODE TACTIC
   else {
      nextBestMove = bestExplodeMove(g);
   }

   return nextBestMove;
}


