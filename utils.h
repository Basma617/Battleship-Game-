#ifndef UTILS_H
#define UTILS_H
#include "types.h"



void clearGrid(int g[GRID][GRID]);
void initPlayer(Player *p);
void initAIState(AIState *ai);
void initGameState(GameState *gs);
int isAreaFree(int g[GRID][GRID], int x, int y, int width, int height);
int canPlaceShip(int g[GRID][GRID], int x, int y, int size, int vertical);
int canPlaceShip5x2Manual(int g[GRID][GRID], int x, int y, int vertical);
void placeShip5x2Manual(int g[GRID][GRID], int x, int y, int shipId, int vertical);
void placeShips(int g[GRID][GRID], ShipConfig *config);
int allDestroyed(int g[GRID][GRID]);
int isShipSunk(int g[GRID][GRID], int shipId);
void markShipSunk(int g[GRID][GRID], int shipId);
Player* getCurrentPlayer(int mode, GameState *gs);
Player* getOpponent(int mode, GameState *gs);
void showInfoBox(SDL_Window *window, const char *format, int a, int b);
ScoreMode getScoreModeFromGameMode(int mode);


#endif