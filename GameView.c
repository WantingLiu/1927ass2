// GameView.c ... GameView ADT implementation

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "Globals.h"
#include "Game.h"
#include "GameView.h"
#include "Graph.h"
#include "Queue.h"

#define HEALTH 0

//====static functions====


struct gameView {

	Graph map;
	int turnNum;
	int health[NUM_PLAYERS];
	int trails[NUM_PLAYERS][TRAIL_SIZE];
	int score;
	LocationID encounters[TRAIL_SIZE];
	LocationID immatureVamp;
};

static void makeActionHunter(GameView g, PlayerID p, char a);
static int makeActionDracula(GameView g, PlayerID p, char a, int index);
static void addTrail(GameView g, PlayerID p, LocationID loc);
	
	
Queue railLocs(Graph map, Queue search, LocationID *prevBase, int recurse);
int inArray (LocationID *arr, LocationID loc);
int makeList(int *locs, Queue q);

static void addTrail(GameView g, PlayerID p, LocationID loc);
static void addEncounter(GameView g, LocationID loc);
static void removeEncounter(GameView g, PlayerID p);
static int getIsDead(GameView currentView, PlayerID player);
//===============================
//			Static Functions
//===============================



static void addTrail(GameView g, PlayerID p, LocationID loc)
{	
	int i;
	for (i = TRAIL_SIZE-2; i>=0; i--) {
		g->trails[p][i+1] = g->trails[p][i];
	}
	
	g->trails[p][0] = loc;
}

static void addEncounter(GameView g, LocationID loc)
{	
	int i;
	for (i = TRAIL_SIZE-2; i>=0; i--) {
		g->encounters[i+1] = g->encounters[i];
	}
	
	g->encounters[0] = loc;
}

static void removeEncounter(GameView g, PlayerID p)
{	
	int i;
	for (i=0; i<TRAIL_SIZE; i++) {
		if(g->encounters[i] == g->encounters[0]) {
			g->encounters[i] = NOWHERE;
			break;
		}
	}
}

static void makeActionHunter(GameView g, PlayerID p, char a)
{

	int lifeLoss = 0;

	switch (a) {
		case '.':
			break;
		case 'T':
			removeEncounter(g, p);
			lifeLoss = LIFE_LOSS_TRAP_ENCOUNTER;
			break;
		case 'V':
			g->immatureVamp = NOWHERE;
			break;
		case 'D':
			lifeLoss = LIFE_LOSS_DRACULA_ENCOUNTER;
			g->health[PLAYER_DRACULA] -= LIFE_LOSS_HUNTER_ENCOUNTER;
	}

	g->health[p] -= lifeLoss;
	
	if (g->health[p] <= 0) {
		g->health[p] = -1;
		g->score -= 6;
	}
}
	
	
static int makeActionDracula(GameView g, PlayerID p, char a, int index)
{

	int added = FALSE;
	switch (a) {
		case '.':		//nothing happens
			break;
		case 'T':		//trap is made by game
			addEncounter(g,g->trails[PLAYER_DRACULA][0]);
			added = TRUE;
			break;
		case 'M':		//trap is destroyed by game
			break;
		case 'V':		//immature vampire matures
			if (index == 5){
				g->score -= 13;
			} else {
				g->immatureVamp = g->trails[PLAYER_DRACULA][0];
			}
			break;
	}
	return added;
}


//===============================
//			Interface Functions
//===============================

// Creates a new GameView to summarise the current state of the game
GameView newGameView(char *pastPlays, PlayerMessage messages[])
{
	GameView g = malloc(sizeof(struct gameView));
	
	g->map = newGraph(NUM_MAP_LOCATIONS);	
	addConnections(g->map);

	//printf("Game is at %p and map is at %p\n",g,g->map);
	
	//make score
	g->score = GAME_START_SCORE;
	
	//init turn number
	g->turnNum = 0;
	
	//initialises player and dracular health
	g->health[PLAYER_LORD_GODALMING] = GAME_START_HUNTER_LIFE_POINTS;
	g->health[PLAYER_DR_SEWARD] = GAME_START_HUNTER_LIFE_POINTS;
	g->health[PLAYER_VAN_HELSING] = GAME_START_HUNTER_LIFE_POINTS;
	g->health[PLAYER_MINA_HARKER] = GAME_START_HUNTER_LIFE_POINTS;
	g->health[PLAYER_DRACULA] = GAME_START_BLOOD_POINTS;
	
	int sl;
	for (sl = 0; sl < NUM_PLAYERS; sl++) {
		int i;
		for (i=0; i<TRAIL_SIZE; i++) {
			g->trails[sl][i] = UNKNOWN_LOCATION;
		}
	}
	
	g->immatureVamp = NOWHERE;
	
	//go forward through past plays and make actions
	int curr = 0;
	while (pastPlays[curr] != '\0') {
	
		//printf("evaluating %c pos %d\n", pastPlays[curr], curr);
		//determine which player
		PlayerID p;
		if (pastPlays[curr] == 'G') {p = PLAYER_LORD_GODALMING;}
		else if (pastPlays[curr] == 'S') {p = PLAYER_DR_SEWARD;}
		else if (pastPlays[curr] == 'H') {p = PLAYER_VAN_HELSING;}
		else if (pastPlays[curr] == 'M') {p = PLAYER_MINA_HARKER;}
		else if (pastPlays[curr] == 'D') {p = PLAYER_DRACULA;}
		else {goto EndWhile;}
			
		if(getIsDead(g,p)) g->health[p] = GAME_START_HUNTER_LIFE_POINTS;
		//get location id from string from lab
		//using Places.c
		char abbrev[3];
		abbrev[0] = pastPlays[curr+1];
		abbrev[1] = pastPlays[curr+2];
		abbrev[2] = '\0';
		if (p == PLAYER_DRACULA) {
			printf("evaluating dracs move: %s\n",abbrev);
			if (!strcmp("C?",abbrev)) addTrail(g,p,CITY_UNKNOWN);
			else if (!strcmp("S?",abbrev)) {
				addTrail(g,p,SEA_UNKNOWN);
				g->health[PLAYER_DRACULA] -= 2;				
			} else if (!strcmp("HI",abbrev)) addTrail(g,p,HIDE);
			else if (abbrev[0] == 'D') {
				int backID =  abbrev[1] - '0';
				printf("doubling back to %c %d\n", abbrev[1],backID);
				addTrail(g,p,102+backID);
				if(SEA_UNKNOWN == g->trails[PLAYER_DRACULA][backID]) g->health[PLAYER_DRACULA] -= 2;
				else if (SEA == idToType(g->trails[PLAYER_DRACULA][backID])) g->health[PLAYER_DRACULA] -= 2;
			} else {
				LocationID loc = abbrevToID(abbrev);
				addTrail(g, p, loc);
				if(idToType(loc) == SEA) g->health[PLAYER_DRACULA] -= 2;
			}
		} else {
			LocationID loc = abbrevToID(abbrev);
			addTrail(g, p, loc);
			
		}
		
		//make actions
		int index;
		if (p == PLAYER_DRACULA) {
			for (index = 3; index<7; index++) {
				if (makeActionDracula(g, p, pastPlays[curr+index], index) == FALSE) addEncounter(g,NOWHERE);	//makes the action on the dracula	
			}
		} else {
			for (index = 3; index<7; index++) {
				makeActionHunter(g, p, pastPlays[curr+index]);			//makes the action on the player	
			}
		}
		
		if (p==PLAYER_DRACULA) {
			g->score--;
				printf("drac ends turn at %d<============\n", getLocation(g,PLAYER_DRACULA));
		}
		
		curr += 8;
		g->turnNum++;
	}
	
	EndWhile: ;
	
	return g;
}


	

// Frees all memory previously allocated for the GameView toBeDeleted
void disposeGameView(GameView toBeDeleted)
{
	//printf("Game is at %p and map is at %p\n",toBeDeleted,toBeDeleted->map);
	dropGraph(toBeDeleted->map);
	free(toBeDeleted);
}


//// Functions to return simple information about the current state of the game

// Get the current round
Round getRound(GameView currentView)
{
	return currentView->turnNum/5;
}

// Get the id of current player - ie whose turn is it?
PlayerID getCurrentPlayer(GameView currentView)
{
	return currentView->turnNum%5;
}

// Get the current score
int getScore(GameView currentView)
{
	return currentView->score;
}

// Get the current health points for a given player
int getHealth(GameView currentView, PlayerID player)
{
	if (getIsDead(currentView,player)) return GAME_START_HUNTER_LIFE_POINTS;
	return currentView->health[player];
}

static int getIsDead(GameView currentView, PlayerID player)
{
	if (currentView->health[player] < 1) return TRUE;
	return FALSE;
}


// Get the current location id of a given player
LocationID getLocation(GameView currentView, PlayerID player)
{
	if (getIsDead(currentView, player)) return ST_JOSEPH_AND_ST_MARYS;
	return currentView->trails[player][0];
}

//// Functions that return information about the history of the game

// Fills the trail array with the location ids of the last 6 turns
//first location is most recent
void getHistory(GameView currentView, PlayerID player,
	                        LocationID trail[TRAIL_SIZE])
{
	int i;
	for (i = 0; i<TRAIL_SIZE; i++) {
		trail[i] = currentView->trails[player][i];
	}
}

//// Functions that query the map to find information about connectivity
// Returns an array of LocationIDs for all directly connected locations

LocationID *connectedLocations(GameView currentView, int *numLocations,
	                           LocationID from, PlayerID player, Round round,
	                           int road, int rail, int sea)
{
	LocationID *edges = malloc(NUM_MAP_LOCATIONS*sizeof(LocationID));
	int c;
	for (c=0;c<NUM_MAP_LOCATIONS;c++) {
		edges[c] = -1;
	}
	
	LocationID to;
	Queue locQ = newQueue();
	QueueJoin(locQ,from);
	for (to=0;to<getnV(currentView->map);to++) {
		int val = getGraphElement(currentView->map,from,to);
		//printf("%s to %s is %d\n",idToName(from),idToName(to),val);
		if (val >= 4) {
			val -= 4;
			if (sea) {
				QueueJoin(locQ,to);
				//printf("adding %s to found (boat)\n",idToName(to));
			}
		} 
		if (val >= 2) {
			val -= 2;
			if (rail) {
				Queue search = newQueue();
				QueueJoin(search,from);
				LocationID *prevBase = calloc(NUM_MAP_LOCATIONS,sizeof(int));
				int recurse = (round + player) % 4;
				//printf("recurse is %d\n", recurse);
				if (recurse) {
					Queue tmp = railLocs(currentView->map, search, prevBase, recurse);
					//printf("tmp is "); showQueue(tmp);
					QueueCat(locQ,tmp);
				}
			}
		} 
		if (val >= 1) {
			if (road) {
				QueueJoin(locQ,to);
				//printf("adding %s to found (road)\n",idToName(to));
			}
		}
	}
	
	showQueue(locQ);
	
	*numLocations = makeList(edges, locQ);
	
	return edges;
}

int makeList(int *locs, Queue q)
{
	int i = 0;
	//printf("final list is:\n");
	while(!QueueIsEmpty(q)) {
		LocationID val = QueueLeave(q);
		if(!inArray(locs, val)) {
			printf("%s\n",idToName(val));
			locs[i] = val;
			i++;
		}
	}
	return i;
} 

int inArray (LocationID *arr, LocationID loc)
{
	int i;
	for (i=0;i<NUM_MAP_LOCATIONS;i++) {
		//printf("arr[%d] is %d\n", i, arr[i]);
		if(arr[i] == loc) return TRUE;
	}	
	return FALSE;
}

Queue railLocs(Graph map, Queue search, LocationID *prevBase, int recurse)
{
	Queue found = newQueue();
	int j = 0;
	LocationID *prev = malloc(NUM_MAP_LOCATIONS*sizeof(int));
	while (!QueueIsEmpty(search)) {
		LocationID tmpLoc = QueueLeave(search);
		prev[j] = tmpLoc;
		prevBase[tmpLoc] = TRUE;
		j++;
	}
	
	int k;
	int i;
	for(k=0;k<j;k++) {
		for (i=0;i<getnV(map);i++) {
			if (prevBase[i] == TRUE) continue;			
			int val = getGraphElement(map,prev[k],i);
			//printf("%s to %s is %d\n",idToName(prev[k]),idToName(i),val);
			if (val == 2 || val == 3 || val == 6 || val == 7) {
				QueueJoin(found, i);
				//printf("adding %s to found (rail) <=========\n",idToName(i));
			}
		}
	}

	recurse--;
	if (recurse) found = railLocs(map, found, prevBase, recurse);
	return found;
}

int giveMeTurnNumber(GameView g)
{
	return g->turnNum;
}

Graph getGameMap (GameView g)
{
	return g->map;
}



