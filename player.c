/**
 * Runs a player's game turn ...
 *
 * Can produce either a Hunter player or a Dracula player
 * depending on the setting of the I_AM_DRACULA #define
 *
 * This is a dummy version of the real player.c used when
 * you submit your AIs. It is provided so that you can
 * test whether your code is likely to compile ...
 *
 * Note that it used to drive both Hunter and Dracula AIs.
 * It first creates an appropriate view, and then invokes the
 * relevant decide-my-move function, which should use the 
 * registerBestPlay() function to send the move back.
 *
 * The real player.c applies a timeout, so that it will halt
 * decide-my-move after a fixed amount of time, if it doesn't
 * finish first. The last move registered will be the one
 * used by the game engine. This version of player.c won't stop
 * your decide-my-move function if it goes into an infinite
 * loop. Sort that out before you submit.
 *
 * Based on the program by David Collien, written in 2012
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Game.h"
#ifdef I_AM_DRACULA
#include "DracView.h"
#include "dracula.h"
#else
#include "HunterView.h"
#include "hunter.h"
#endif

// moves given by registerBestPlay are this long (including terminator)
#define MOVE_SIZE 3

// The minimum static globals I can get away with
static char latestPlay[MOVE_SIZE] = "";
static char latestMessage[MESSAGE_SIZE] = "";

int main(int argc, char *argv[])
{
	printf("check\n");
#ifdef I_AM_DRACULA
   DracView gameState;
   char *plays = "GZA.... SED.... HZU.... MZU....";
   PlayerMessage msgs[3] = { "", "", "" };
   gameState = newDracView(plays,msgs);
   decideDraculaMove(gameState);
   disposeDracView(gameState);
#else
   HunterView gameState;
   char *plays = "GED.... SBO.... HCD.... MMI.... DC?.V.. GMN.... SBB.... HGA.... MMR.... DC?T... GED.... SAO.... HCN.... MMS.... DS?.... GMN.... SNS.... HBS.... MAO.... DTS.... GED.... SED.... HIO.... MEC.... DC?T... GMN.... SED.... HTS.... MNS.... DD2.... GED.... SMN.... HMS.... MED.... DMS..V. GNS.... SMN.... HAO....";
   PlayerMessage msgs[100] = {
	"0 . 49 -1",
    "0 . 0 -1",
    "0 . 1 -1",
    "0 . 2 -1",
    "",
    "1 . 3 -1",
    "0 . 1 -1",
    "0 . 2 -1",
    "0 . 3 -1",
    "",
    "2 . 4 -1",
    "0 . 2 -1",
    "0 . 3 -1",
    "0 . 4 -1",
    "+++Oneoneoneoneoneoneone+++",
    "3 . 5 -1",
    "0 . 3 -1",
    "0 . 4 -1",
    "0 . 5 -1",
    "+++MELON MELON MELON+++",
    "4 . 6 -1",
    "0 . 4 -1",
    "0 . 5 -1",
    "0 . 6 -1",
    "+++MELON MELON MELON+++",
    "5 . 7 -1",
    "0 . 5 -1",
    "0 . 6 -1",
    "0 . 7 -1",
    "+++Whoops! Here Comes The Cheese!+++",
    "6 . 8 64",
    "0 . 6 -1",
    "0 . 7 -1",
    "0 . 8 -1",
    "+++Oneoneoneoneoneoneone+++",
    "7 . 9 43",
    "0 . 7 -1",
    "0 . 8 -1"
  };

   gameState = newHunterView(plays,msgs);
   decideHunterMove(gameState);
   disposeHunterView(gameState);
#endif 
   printf("Move: %s, Message: %s\n", latestPlay, latestMessage);
   return EXIT_SUCCESS;
}

// Saves characters from play (and appends a terminator)
// and saves characters from message (and appends a terminator)
void registerBestPlay (char *play, PlayerMessage message) {
   strncpy(latestPlay, play, MOVE_SIZE-1);
   latestPlay[MOVE_SIZE-1] = '\0';
     
   strncpy(latestMessage, message, MESSAGE_SIZE);
   latestMessage[MESSAGE_SIZE-1] = '\0';
}
