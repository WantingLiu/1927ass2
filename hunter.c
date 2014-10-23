// hunter.c
// Implementation of your "Fury of Dracula" hunter AI

#include <stdlib.h>
#include <stdio.h>
#include "Game.h"
#include <time.h>
#include "IntList.h"
#include "Graph.h"
#include "HunterView.h"

#define PLAYER1_START_POS 22
#define PLAYER2_START_POS 11
#define PLAYER3_START_POS 17
#define PLAYER4_START_POS 44

#define SCORE_UNTIL_EXPLODE 10
#define EXPLODE_TURNS 8


int makeRandomMove(HunterView g);
int makeRandomMove2(HunterView h, int p);
LocationID makeLeaderMove (HunterView h, char *out);
int makeFollowerMove (HunterView g, int player, int rank);
int getRank(HunterView h, int player, int round);
int isLeaderExploded (HunterView h, int player);
int isLeaderRested (HunterView h);

//static void makeMessage(HunterView h, int player, char *out);
//static char makeLeaderBehaviourCode (HunterView h);
static void makeMessageFollower(HunterView h, int player, char *out);
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
//rank[0] is rank of lord goldaming NOT rank of leader
//static int ranks[4];

//player messages moved higher as it is needed for hunterView.c which is included
PlayerMessage *messages;


void decideHunterMove(HunterView gameState)
{

	int turn = giveMeTurnNum(gameState);
	//don't need to malloc as PlayerMessage is a type of char[100]
	messages = malloc(sizeof(PlayerMessage)*turn);
	getMessages(gameState,messages);
	//what is this line for
	srand(time(NULL));
	

	int current_player = whoAmI(gameState);
	//initialises current_leader global variable
	int current_round = giveMeTheRound(gameState);
	int current_rank = getRank(gameState, current_player, current_round);
	PlayerMessage message = "";

	int move = 0;
	char *moveTo;

	if (current_round == 0) {
		if(current_player == PLAYER_LORD_GODALMING) {move = PLAYER1_START_POS;}
		else if(current_player == PLAYER_DR_SEWARD) {move = PLAYER2_START_POS;}
		else if(current_player == PLAYER_VAN_HELSING) {move = PLAYER3_START_POS;}
		else if(current_player == PLAYER_MINA_HARKER) {move = PLAYER4_START_POS;}
		else printf("I am noone..\n");
		makeMessageFollower(gameState, current_player, message);
	} else if (current_player == current_leader) {
		printf("I am the leader\n");
		move = makeRandomMove2(gameState,current_player);
		moveTo = idToAbbrev(move);
		registerBestPlay(moveTo,message);
		
		
		move = makeLeaderMove(gameState,message);
	} else {
		printf("I am a follower\n");
		move = makeRandomMove2(gameState,current_player);
		moveTo = idToAbbrev(move);
		registerBestPlay(moveTo,message);
		
		
		move = makeFollowerMove(gameState, current_player, current_rank);
		makeMessageFollower(gameState, current_player, message);
	}
	
	printf("move is %d\n",move);
	
	moveTo = idToAbbrev(move);
	registerBestPlay(moveTo,message);
}

// generate a random move where no hunter currently is
int makeRandomMove(HunterView h)
{
	int numLocations;
	int *locs = whereCanIGo(h,&numLocations,TRUE,TRUE,FALSE);
	int selectRandIndex = (rand()%(numLocations-1))+1; //so we don't select current loc (element 0)
	return locs[selectRandIndex];
}

int makeRandomMove2(HunterView h, int player)
{
	int move;
	int flag1 = TRUE;
	int flag2;
	while(flag1) {
		flag2 = FALSE;
		int r_move = makeRandomMove(h);
		int i;
		for (i=0;i<NUM_PLAYERS-1;i++) {
			if (i != player && whereIs(h,i) == r_move) {
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

LocationID makeLeaderMove (HunterView h, char *out)
{
	//'C' or '.' form conga
	//'E' explode
	//'H' hunt TODO
	
	int move;
	
	int ignore;
	
	char incode;
	char outcode;
	
	int inactionTurns;
	int outactionTurns;
	
	int intarget;
	int outtarget;
	
	int player = current_leader;
	int turn = giveMeTurnNum(h);
	int myLoc = whereIs(h,player);
	
	LocationID dracTrail[TRAIL_SIZE];
	giveMeTheTrail(h,PLAYER_DRACULA,dracTrail);
	
	sscanf(messages[turn-NUM_PLAYERS],"%d %c %d %d",&inactionTurns,&incode,&ignore,&intarget);
	
	//GENERATES move, outactionturns, outcode and out target
	if (incode == 'E' && inactionTurns < EXPLODE_TURNS) {//continue exploding
		printf("continue explode\n");
		move = explode(h,player);
		outactionTurns = inactionTurns+1;
		outcode = 'E';
		outtarget = -1;
	} else if (incode == 'R') { //after a rest, reset target to that location
		printf("locking onto drac\n");
		int path[NUM_MAP_LOCATIONS];
		findHunterPath(h, myLoc, dracTrail[5], path, TRUE, TRUE, TRUE);
		move = path[0];
		outactionTurns = inactionTurns+1;
		outcode = '.';
		outtarget = dracTrail[5];
	} else if (incode == 'E' && inactionTurns == EXPLODE_TURNS) { //rest
		printf("explode got nowhere. Resting!\n");
		move = whereIs(h,player);
		outactionTurns = 0;
		outcode = 'R';
		outtarget = -1;
	} else if (myLoc == intarget) { //decide when to explode
		printf("INITIATE EXPLODE\n");
		move = explode(h,player);
		outactionTurns = 0;
		outcode = 'E';
		outtarget = -1;
	} else {
		printf("Business as usual\n");
		
		int tmpLoc = -1;
		int j;
		for(j=0;j<TRAIL_SIZE;j++) {
			if (dracTrail[j] >= MIN_MAP_LOCATION && dracTrail[j] <= MAX_MAP_LOCATION) {
				tmpLoc = dracTrail[j];
				break;
			}
		}
		
		if (tmpLoc != -1) outtarget = tmpLoc;
		else if (intarget != -1) outtarget = intarget;
		else outtarget = -1;		
		
		outactionTurns = inactionTurns+1;
		outcode = '.';
		
		if (outtarget != -1) {
			int path[NUM_MAP_LOCATIONS];
			findHunterPath(h, myLoc, outtarget, path, TRUE, TRUE, TRUE);
			move = path[1];
		} else move = makeRandomMove2(h,player);
	}
	
	
	//GENERATES outscore
	int inscore = 0;
	int outscore = 0;
	if(giveMeTheRound(h) == 1 && player == PLAYER_LORD_GODALMING) {
		outscore = 3;
	} else {
		int currLoc;
		if (player == PLAYER_LORD_GODALMING) currLoc = whereIs(h,PLAYER_MINA_HARKER);
		else 								 currLoc = whereIs(h,(player-1));

		int foundDrac = FALSE;
		
		int i;
		for(i=0;i<TRAIL_SIZE;i++) {
			if (currLoc == dracTrail[i]) {
				outscore = i;
				foundDrac = TRUE;
				printf("someone is on drac's trail[%d] location: %d\n",i,currLoc);
				break;
			}
		}
	
		int ignore1;
		char ignore2;
		int ignore3;
		
		if (!foundDrac) {
			printf("no one is on drac's trail\n");			
			sscanf(messages[turn-NUM_PLAYERS],"%d %c %d %d",&ignore1,&ignore2,&inscore,&ignore3);
			outscore = inscore + 1;
		}
	}
	
	printf("inscore: %d outscore: %d\n",inscore,outscore);
	sprintf(out,"%d %c %d %d",outactionTurns,outcode,outscore,outtarget);
	
	return move;
}



//Conga Line AI
//follow the Leader AI's trail - each minion is assigned a particular index Leader's trail
int makeFollowerMove (HunterView h, int player, int rank)
{
	int move;
	if (isLeaderRested(h)) {
		printf("Leader is rested\n");
		//rest as well
		move = whereIs(h, player);
	} else if (isLeaderExploded(h,player)) {
		move = explode(h,player);
	} else {
		
		//make the leaders trail
		int leader_trail[TRAIL_SIZE];
		giveMeTheTrail(h,current_leader,leader_trail);
		//make the path from the current player to the rank'th' element of the leaders trail
		int path[NUM_MAP_LOCATIONS];
		while (leader_trail[rank] == -1) {
			rank--;
		}
		findHunterPath(h, whereIs(h,player), leader_trail[rank], path, TRUE, TRUE, TRUE);
		printf("I am at %d. I am rank %d. I am trying to get to %d\n",whereIs(h,player),rank,leader_trail[rank]);
		move = path[1];
	}
	return move;
}




int getRank(HunterView h, int player, int round)
{
	int rank = 0;
	
	if (round == 0) {
		if(player == PLAYER_LORD_GODALMING) {rank = -1;}
		else if(player == PLAYER_DR_SEWARD) {rank = 0;}
		else if(player == PLAYER_VAN_HELSING) {rank = 1;}
		else if(player == PLAYER_MINA_HARKER) {rank = 2;}
		else printf("I am noone..\n");		
	} else if (round == 1 && player == PLAYER_LORD_GODALMING) {
		rank = 3;
	} else {
		int turn = giveMeTurnNum(h);

		IntList l = newIntList();
	
		int foundDrac = FALSE;
		LocationID dracTrail[TRAIL_SIZE];
		giveMeTheTrail(h,PLAYER_DRACULA,dracTrail);

		int currLoc;	
		if (player == PLAYER_LORD_GODALMING) currLoc = whereIs(h,PLAYER_MINA_HARKER);
		else currLoc = whereIs(h,player-1);
	
		int j;
		for(j=0;j<TRAIL_SIZE;j++) {
			if (currLoc == dracTrail[j]) {
				foundDrac = TRUE;
				break;
			}
		}
	
		int score;
		int ignore1;
		char ignore2;
		int playertmp = 2;
		int insertPlayer;
	
		int i;
		for (i=1;i<6;i++) {
			if (i != player+1) { //skips drac
				//printf("scanning message %s\n",messages[turn-i]);
				//printf("inserting into list player: %d score: %d\n",(player+5-playertmp)%5,tmp);
				if (playertmp==4 && foundDrac) score = j;
				else sscanf(messages[turn-i],"%d %c %d",&ignore1,&ignore2,&score);
				insertPlayer = (player+4-playertmp)%4;
				IntListInsertInOrder(l,score,insertPlayer);
				playertmp++;
			}
		}
	
		//showList(l);
	
		current_leader = getFirstPlayer(l);
		rank = playerPos(l,player);
	}
	
	return rank;
}

/*
void makeRank
{

}
*/
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
	char tmp;
	int ignore;
	int offset;
	
	if(current_leader<player) { //before drac
		offset = turn-3+current_leader;
	} else { //need to skip drac
		offset = turn-4+current_leader;
	}
	sscanf(messages[offset],"%c%n",&tmp,&ignore);
	
	return (tmp=='E');
}

//-===================================================-//


//makes the message of a follower
static void makeMessageFollower(HunterView h, int player, char *out)
{
	int round = giveMeTheRound(h);
	int score = 0;
	if(giveMeTheRound(h) == 0) {
		if(player == PLAYER_LORD_GODALMING) {score = -1;}
		else if(player == PLAYER_DR_SEWARD) {score = 0;}
		else if(player == PLAYER_VAN_HELSING) {score = 1;}
		else if(player == PLAYER_MINA_HARKER) {score = 2;}
	}else if (round == 1 && player == PLAYER_LORD_GODALMING) {
		score = 3;
	} else {
		
		int currLoc;
		if (player == PLAYER_LORD_GODALMING) currLoc = whereIs(h,PLAYER_MINA_HARKER);
		else currLoc = whereIs(h,player-1);
		int foundDrac = FALSE;
		LocationID dracTrail[TRAIL_SIZE];
		giveMeTheTrail(h,PLAYER_DRACULA,dracTrail);
		
		int i;
		for(i=0;i<TRAIL_SIZE;i++) {
			if (currLoc == dracTrail[i]) {
				score = i;
				foundDrac = TRUE;
				break;
			}
		}
	
		int ignore1;
		char ignore2;
		int ignore3;
		
		int turn = giveMeTurnNum(h);

		if (!foundDrac) {
			sscanf(messages[turn-NUM_PLAYERS],"%d %c %d %d",&ignore1,&ignore2,&score,&ignore3);
			score++;
		}
	}
	sprintf(out,"0 . %d -1",score);
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
	findHunterPath(g, whereIs(g,player), CASTLE_DRACULA, path, TRUE, TRUE, TRUE);

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
		if (distToCast < minDist && closestPlayer != current_leader) {
			closestPlayer = p;
		}
	}

	return closestPlayer;
}


							
			 
