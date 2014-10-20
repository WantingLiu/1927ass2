// hunter.c
// Implementation of your "Fury of Dracula" hunter AI

#include <stdlib.h>
#include <stdio.h>
#include "Game.h"
#include "HunterView.h"
#include <time.h>
#include "IntList.h"
#include "Graph.h"

int makeRandomMove(HunterView g);
int makeLeaderMove (HunterView g);
int makeFollowerMove (HunterView g, int player, int rank);
int getRank(HunterView h, int player);
int isLeaderExploded (HunterView h, int player);
int isLeaderRested (HunterView h);
static void makeMessage(HunterView h, int player, char *out);
static char makeLeaderBehaviourCode (HunterView h);

static LocationID explode(HunterView h, int player);
static LocationID bestExplodeMove (HunterView g, int currPlayer);
static LocationID explodeToCastle (HunterView g, PlayerID player);
static PlayerID whoIsClosestToCastle (HunterView g);

/*
Possible stategy:
	1) Place players scattered over the map, with Lord Goldamin in the middle of the map
	2) Players converge to Lord Goldaming (initial default Leader), do this for 6 turns
	3) Every 6 turns, all players rest a turn to reveal Dracula's 6th location of his trail
	4) Leader moves towards the revealed location, other players converge/follow the leader
	5) At any point in the game, if any hunter lands on Dracula's trail, make that hunter the new leader.
		5.1) Every player has a "Leader Value". The "Leader Value" is determined by 
			[(Dracula's trail location that hunter has landed on last)+num of turns passed after that] which needs to be reset when relevant
	
*/


static int current_leader = 0;

void decideHunterMove(HunterView gameState)
{
	srand(time(NULL));
	
	int move;
	int current_player = whoAmI(gameState);
	int current_rank = getRank(gameState, current_player);
	int current_round = giveMeTheRound(gameState);
	if (current_round == '0') {
		if(current_player == PLAYER_LORD_GODALMING) {move = 1;} //TODO: change starting locations to something better
		else if(current_player == PLAYER_DR_SEWARD) {move = 2;}
		else if(current_player == PLAYER_VAN_HELSING) {move = 3;}
		else if(current_player == PLAYER_MINA_HARKER) {move = 4;}
	} else if (current_player == current_leader) {
		makeLeaderMove(gameState);
	} else {	
		move = makeFollowerMove(gameState, current_player, current_rank);
		//move = makeRandomMove(gameState, current_player);
	}
	
	PlayerMessage message;
	makeMessage(gameState, current_player, message);

	char *moveTo = idToAbbrev(move);
	registerBestPlay(moveTo,message);
}

// generate a random move where no hunter currently is
int makeRandomMove(HunterView h)
{
	int *numLocations = NULL;
	int *locs = whereCanIGo(h,numLocations,TRUE,TRUE,FALSE);
	int selectRandIndex = (rand()%(*numLocations-1))+1; //so we don't select current loc (element 0)
	return locs[selectRandIndex];
}


//Leader AI
//Leader has missions
//Stage 1: Move randomly ala conga line
//Stage 2: Incorporate resting, moving to places, and exploding
int makeLeaderMove (HunterView h)
{
	int move;
	int flag1 = TRUE;
	int flag2;
	while(flag1) {
		flag2 = FALSE;
		int r_move = makeRandomMove(h);
		int i;
		for (i=0;i<NUM_PLAYERS-1;i++) {
			if (whereIs(h,i) == r_move) {
				flag2 = TRUE;
			}
		}
		if (flag2 == FALSE) {
			move = r_move;
			flag1 = FALSE;
		}
	}
	return move;
}



//Conga Line AI
//follow the Leader AI's trail - each minion is assigned a particular index Leader's trail
int makeFollowerMove (HunterView h, int player, int rank)
{
	int move;
	if (isLeaderRested(h)) {
		move = whereIs(h, player);
	} else if (isLeaderExploded(h,player)) {
		move = explode(h,player);
	} else {
		//make the leaders trail
		int leader_trail[TRAIL_SIZE];
		giveMeTheTrail(h,current_leader,leader_trail);
		//make the path from the current player to the rank'th' element of the leaders trail
		int path[NUM_MAP_LOCATIONS];		
		findPath(h, whereIs(h,player), leader_trail[rank], path, TRUE, TRUE, TRUE);
		move = path[0];
	}
	return move;
}




int getRank(HunterView h, int player)
{
	int turn = giveMeTurnNum(h);
	PlayerMessage messages[turn];
	getMessages(h,messages);
	
	int playertmp = 1;
	IntList l = newIntList();
	
	int tmp;
	int ignore;
	int i;
	for (i=1;i<6;i++) {
		if (i!=player) { //skips drac
			sscanf(messages[turn-i],"%n%d",&ignore,&tmp);
			IntListInsertInOrder(l,tmp,(player+5-playertmp)%5);
			playertmp++;
		}
	}
	
	current_leader = getFirstPlayer(l);
	return playerPos(l,player);
}

//-===================================================-//

int isLeaderRested (HunterView h)
{
	int leader_trail[TRAIL_SIZE];
	giveMeTheTrail(h,current_leader,leader_trail);
	if (leader_trail[0] == leader_trail[1])	return TRUE;
	return FALSE;
}

int isLeaderExploded (HunterView h, int player)
{
	int turn = giveMeTurnNum(h);
	PlayerMessage messages[turn-1];
	getMessages(h, messages);
	
	char tmp;
	int ignore;
	int offset;
	
	//TODO work out maths
	if(current_leader<player) { //before drac
		offset = turn-3+current_leader;
	} else { //need to skip drac
		offset = turn-4+current_leader;
	}
	sscanf(messages[offset],"%c%n",&tmp,&ignore);
	
	return (tmp=='E');
}

//-===================================================-//

static void makeMessage(HunterView h, int player, char *out)
{
	char moveCode = '.';
	if (current_leader == player) {
		moveCode = makeLeaderBehaviourCode(h);
	}

	int turn = giveMeTurnNum(h);
	PlayerMessage messages[turn-1];
	getMessages(h, messages);
	
	int currLoc = whereIs(h,player);
	int foundDrac = FALSE;
	LocationID dracTrail[TRAIL_SIZE];
	giveMeTheTrail(h,PLAYER_DRACULA,dracTrail);
	int i;
	for(i=0;i<TRAIL_SIZE;i++) {
		if (currLoc == dracTrail[i]) {
			sprintf(out,"%c%d",moveCode,dracTrail[i]);
			foundDrac = TRUE;
			break;
		}
	}
	
	int tmp;
	int ignore;
	int offset;
	
	if (!foundDrac) {
		if (player == PLAYER_LORD_GODALMING) {
			offset = turn-3;
		} else {
			offset = turn-2;
		}
		sscanf(messages[offset],"%n%d",&ignore,&tmp);
		sprintf(out,"%c%d",moveCode,tmp+1);
	}
}

static char makeLeaderBehaviourCode (HunterView h)
{
	//TODO logic of when to do what such as when to explode
	//'C' or '.' form conga
	//'E' explode
	//'H' hunt
	
	return '.';
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

static LocationID explode(HunterView h, int player)
{
	LocationID move;
	
	if (player == whoIsClosestToCastle(h)) {
		move = explodeToCastle(h, player);
	} else {
		move = bestExplodeMove(h,player);
	}
	
	return move;
}

static LocationID bestExplodeMove (HunterView g, int currPlayer) 
{
	int numLocations;
	LocationID *possibleMoves = whereCanIGo(g, &numLocations, TRUE, TRUE, TRUE);
	//initially set to player's current location
	LocationID bestMove = possibleMoves[0];
	
	int maxDist = 0;
	int sumDist;
	LocationID moveOption;
	
	int i;	
	for (i=0; i<numLocations; i++) {
		moveOption = possibleMoves[i];
		sumDist = 0;
		PlayerID p;
		for (p=0; p<(NUM_PLAYERS-1); p++) {
			if (p!=currPlayer) {
				sumDist += findPathDist(getHunterMap(g), moveOption, whereIs(g, p));
			}
		}
	}

	if (sumDist > maxDist) {
		bestMove = moveOption;
		maxDist = sumDist;
	}

	return bestMove;
}

//returns the next possible move closest to CASTLE_DRACULA
static LocationID explodeToCastle (HunterView g, PlayerID player)
{
	LocationID path[NUM_MAP_LOCATIONS];
	findPath(g, whereIs(g,player), CASTLE_DRACULA, path, TRUE, TRUE, TRUE);

	return path[0];
}

//returns the playerID of the player whose position is closest to CASTLE_DRACULA
static PlayerID whoIsClosestToCastle (HunterView g) 
{
	PlayerID closestPlayer = 0;
	int minDist = findPathDist(getHunterMap(g), whereIs(g,closestPlayer), CASTLE_DRACULA);

	PlayerID p;
	for (p=0; p<(NUM_PLAYERS-1); p++) {
		int distToCast = findPathDist(getHunterMap(g), whereIs(g,p), CASTLE_DRACULA);
		if (distToCast < minDist) {
			closestPlayer = p;
		}
	}

	return closestPlayer;
}


							
			 
