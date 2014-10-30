// DracView.c ... DracView ADT implementation

#include <stdlib.h>
#include <assert.h>
#include "Globals.h"
#include "Game.h"
#include "GameView.h"
#include "DracView.h"
#include "Graph.h"
#include "Queue.h"
// #include "Map.h" ... if you decide to use the Map ADT

#define IMMATUREVAMP 0
#define TRAP 1
#define NONE_TRAP 2
typedef int EncounterType;

struct encounterType {
    LocationID location;
    EncounterType encounter;
};

struct dracView {
    GameView view;
    struct encounterType encounters[TRAIL_SIZE];
};

// Creates a new DracView to summarise the current state of the game
DracView newDracView(char *pastPlays, PlayerMessage messages[])
{
    int i, curr = 0;
    DracView dracView = malloc(sizeof(struct dracView));
    dracView->view = newGameView(pastPlays, messages);

    for (i = 0; i < TRAIL_SIZE; i++) {
        dracView->encounters[i].location = NOWHERE;
        dracView->encounters[i].encounter = NONE_TRAP;
    }

    char location[3];
    // Tracking location of encounters, specific to Dracula, potentially remove this information from GameView
    while (pastPlays[curr] != 0) {
        if (pastPlays[curr] == 'D') {
            // Getting location of Dracula
            location[0] = pastPlays[curr+1];
            location[1] = pastPlays[curr+2];
            location[2] = '\0';
            if (pastPlays[curr+3] == 'T') {
                for (i = TRAIL_SIZE-2; i>=0; i--) {
                    dracView->encounters[i+1] = dracView->encounters[i];
                }
                dracView->encounters[0].location = abbrevToID(location);
                dracView->encounters[0].encounter = TRAP;
            }
            if (pastPlays[curr+4] == 'V') {
                for (i = TRAIL_SIZE-2; i>=0; i--) {
                    dracView->encounters[i+1] = dracView->encounters[i];
                }
                dracView->encounters[0].location = abbrevToID(location);
                dracView->encounters[0].encounter = IMMATUREVAMP;
            }
        // Case of hunter, worry about running into immature vamp or trap
        } else {
            // Getting location of Hunter
            location[0] = pastPlays[curr+1];
            location[1] = pastPlays[curr+2];
            location[2] = '\0';
            // Case of running into a trap
            if (pastPlays[curr+3] == 'T' || pastPlays[curr+4] == 'V') {
                for (i = 0; i < 5; i++) {
                    if (dracView->encounters[i].location == abbrevToID(location)) {
                        dracView->encounters[i].location = NOWHERE;
                        dracView->encounters[i].encounter = NONE_TRAP;
                    }
                }
            }
        }
        curr += 8;
    }



    return dracView;
}
     
     
// Frees all memory previously allocated for the DracView toBeDeleted
void disposeDracView(DracView toBeDeleted)
{
    disposeGameView(toBeDeleted->view);
    free(toBeDeleted);
}


//// Functions to return simple information about the current state of the game

// Get the current round
Round giveMeTheRound(DracView currentView)
{
    return getRound(currentView->view);
}

// Get the current score
int giveMeTheScore(DracView currentView)
{
    return getScore(currentView->view);
}

// Get the current health points for a given player
int howHealthyIs(DracView currentView, PlayerID player)
{
    return getHealth(currentView->view, player);
}

// Get the current location id of a given player, always returns exact location
LocationID whereIs(DracView currentView, PlayerID player)
{
    LocationID loc = getLocation(currentView->view, player);

    if (loc >= 102 && loc <= 108) {
        LocationID trail[TRAIL_SIZE];
        getHistory(currentView->view, PLAYER_DRACULA, trail);
        switch (loc) {
            // Hide
            case 102:
                loc = trail[1];
                break;
            // Double back 1
            case 103:
                loc = trail[1];
                break;
            // Double back 2
            case 104:
                loc = trail[2];
                break;
            // Double back 3
            case 105:
                loc = trail[3];
                break;
            // Double back 4
            case 106:
                loc = trail[4];
                break;
            // Double back 5
            case 107:
                loc = trail[5];
                break;
            // Teleport Castle Dracula
            case 108:
                loc = CASTLE_DRACULA;
                break;
        }
    }
    return loc;
}

// Get the most recent move of a given player
void lastMove(DracView currentView, PlayerID player,
                 LocationID *start, LocationID *end)
{
    LocationID trail[TRAIL_SIZE];
    getHistory(currentView->view, player, trail);
    *start = trail[1];
    *end = trail[0];
}

// Find out what minions are placed at the specified location
void whatsThere(DracView currentView, LocationID where,
                         int *numTraps, int *numVamps)
{
    *numTraps = 0;
    *numVamps = 0;
    if (where == NOWHERE || where == SEA) {
        // Do nothing
    } else {
        int i;
        for (i = 0; i < 5; i++) {
            if (currentView->encounters[i].encounter == TRAP && currentView->encounters[i].location == where) {
                (*numTraps)++;
            } else if (currentView->encounters[i].encounter == IMMATUREVAMP && currentView->encounters[i].location == where) {
                (*numVamps)++;
            }
        }
    }
}

//// Functions that return information about the history of the game

// Fills the trail array with the location ids of the last 6 turns
void giveMeTheTrail(DracView currentView, PlayerID player,
                            LocationID trail[TRAIL_SIZE])
{
    getHistory(currentView->view, player, trail);
}

//// Functions that query the map to find information about connectivity

// What are my (Dracula's) possible next moves (locations)
LocationID *whereCanIgo(DracView currentView, int *numLocations, int road, int sea)
{
    // Boolean of whether doubleBack is in the trail
    int doubleBack = FALSE;
    // Boolean of whether hide is in the trail
    int hide = FALSE;
    int temp = 0;
    // New set of edges if case of doubleBack or hide
    LocationID *newEdges = malloc(NUM_MAP_LOCATIONS*sizeof(LocationID));
    LocationID *edges;
    LocationID trail[TRAIL_SIZE];
    // Getting the trail of Dracula
    getHistory(currentView->view, PLAYER_DRACULA, trail);

    int i = 0, j = 0;
    // Case of having a double back or a hide in trail
    while (i < 6) {
        if (trail[i] >= 102 && trail[i] < 108) {
            if (trail[i] > 102 && trail[i] < 108) {
                doubleBack = TRUE;
            }
            if (trail[i] == 102) {
                hide = TRUE;
            }
        } 
        i++;
    }

    // Constructing preliminary edges array
    edges = connectedLocations(currentView->view, numLocations, whereIs(currentView, PLAYER_DRACULA), PLAYER_DRACULA, getRound(currentView->view), road, FALSE, sea);
    temp = *numLocations;
    printf("numLocations1:%d\n", *numLocations);
    printf("edges[1]:%d\n", edges[1]);


    // Case if double back or hide is in the trail, can't have anything in edges array that is in trail
    if (doubleBack == TRUE && hide == TRUE) {
        printf("Doublebackhide\n");
        for (i = 0; i < temp; i++) {
            printf("looped\n");
            if (edges[i] != trail[0] && edges[i] != trail[1] && edges[i] != trail[2] && edges[i] != trail[3] && edges[i] != trail[4] && edges[i] != 60) {
                newEdges[j] = edges[i];
                j++;
            } else {
                // If an invalid location is found, reduce number of locations
                (*numLocations)--;
            }
            printf("edges[i](loop):%d\n", edges[i]);
        }
        (*numLocations) = j;
        edges = newEdges;
    // Case if only doubleBack in trail, can't have anything in trail array apart from most recent location but only if it is not sea (can't hide there)
    } else if (doubleBack == TRUE) {
        printf("Doubleback\n");
        // If the first move in the trail is a double back, special case
        if (trail[0] > 102 && trail[0] < 107) {
            // Calculating how far back the double back goes to
            int howFarBack = (5-(107-trail[0]));

            for (i = 0; i < temp; i++) {
                // If edges contains the double back location, include it since you can hide there
                if (edges[i] == trail[howFarBack]) {
                    if (idToType(trail[howFarBack] != SEA)) {
                        newEdges[j] = edges[i];
                        j++;
                    }
                } else if (edges[i] != trail[1] && edges[i] != trail[2] && edges[i] != trail[3] && edges[i] != trail[4] && edges[i] != 60) {
                    newEdges[j] = edges[i];
                    j++;
                } else {
                    // If an invalid location is found, reduce number of locations
                    (*numLocations)--;
                }
            }
            
        } else {
            printf("Hiiii\n");
            for (i = 0; i < temp; i++) {
                if (edges[i] != trail[1] && edges[i] != trail[2] && edges[i] != trail[3] && edges[i] != trail[4] && edges[i] != 60) {
                    if (edges[i] == trail[0]) {
                        if (idToType(trail[0]) != SEA) {
                            newEdges[j] = edges[i];
                            j++;
                        }
                    } else {
                        newEdges[j] = edges[i];
                        j++;
                    }
                } else {
                    // Invalid location, reduce number of locations possible
                    (*numLocations)--;
                }
            }
        }
        (*numLocations) = j;
        edges = newEdges;
    } else {
        for (i = 0; i < temp; i++) {
            if (edges[i] != 60) {
                newEdges[j] = edges[i];
                j++;
            } else {
                (*numLocations)--;
            }
        }
        edges = newEdges;
        (*numLocations) = j;
    }

    printf("newEdges[0]:%d\n", newEdges[0]);
    return edges;
}

// What are the specified player's next possible moves
LocationID *whereCanTheyGo(DracView currentView, int *numLocations,
                           PlayerID player, int road, int rail, int sea)
{
    LocationID *edges;
    if (player == PLAYER_DRACULA) {
        edges = whereCanIgo(currentView, numLocations, road, sea);
    } else {
        edges = connectedLocations(currentView->view, numLocations, getLocation(currentView->view, player), player, getRound(currentView->view), road, rail, sea);
    }
    return edges;
}
