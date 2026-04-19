#include <SDL3/SDL.h> /*grid et placement des bateauxS*/
#include <stdio.h>
#include "types.h"

#include "utils.h"
void clearGrid(int g[GRID][GRID]) {
    for (int i = 0; i < GRID; i++)
        for (int j = 0; j < GRID; j++)
            g[i][j] = 0;
}

void initPlayer(Player *p) {
    clearGrid(p->grid);
    p->score = 0;
}

void initAIState(AIState *ai) {
    ai->lastHitX = -1;
    ai->lastHitY = -1;
    ai->anchorX = -1;
    ai->anchorY = -1;
    ai->currentX = -1;
    ai->currentY = -1;
    ai->direction = 0;
    clearGrid(ai->shots);
}

void initGameState(GameState *gs) {
    gs->currentPlayer = 0;
    gs->waitingNextPlayer = 1;
    gs->waitingDelay = 0;
    gs->waitingPvPDelay = 0;
    gs->lastActionTime = 0;
    gs->lastPlayerActionTime = 0;
}

int isAreaFree(int g[GRID][GRID], int x, int y, int width, int height) {
    for (int i = x - 1; i <= x + height; i++) {
        for (int j = y - 1; j <= y + width; j++) {
            if (i < 0 || i >= GRID || j < 0 || j >= GRID)
                continue;
            if (g[i][j] != 0)
                return 0;
        }
    }
    return 1;
}

int canPlaceShip(int g[GRID][GRID], int x, int y, int size, int vertical) {
    int width  = vertical ? 1 : size;
    int height = vertical ? size : 1;

    if (x + height > GRID || y + width > GRID)
        return 0;

    return isAreaFree(g, x, y, width, height);
}

int canPlaceShip5x2Manual(int g[GRID][GRID], int x, int y, int vertical) {
    int width  = vertical ? 2 : 5;
    int height = vertical ? 5 : 2;

    if (x + height > GRID || y + width > GRID)
        return 0;

    return isAreaFree(g, x, y, width, height);
}

void placeShip5x2Manual(int g[GRID][GRID], int x, int y, int shipId, int vertical) {
    int width  = vertical ? 2 : 5;
    int height = vertical ? 5 : 2;

    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            g[x + i][y + j] = shipId;
}

void placeShips(int g[GRID][GRID], ShipConfig *config) {
    int shipId = 1;

    for (int s = 0; s < SHIP_COUNT; s++) {
        if (config->sizes[s] > 0) {
            int size = config->sizes[s];
            int placed = 0;

            while (!placed) {
                int vertical = myRand() % 2;
                int width  = vertical ? 1 : size;
                int height = vertical ? size : 1;

                int maxX = GRID - height;
                int maxY = GRID - width;

                int x = myRand() % (maxX + 1);
                int y = myRand() % (maxY + 1);

                if (isAreaFree(g, x, y, width, height)) {
                    for (int i = 0; i < height; i++)
                        for (int j = 0; j < width; j++)
                            g[x + i][y + j] = shipId;
                    placed = 1;
                }
            }
        } else {
            int placed = 0;
            while (!placed) {
                int vertical = myRand() % 2;
                int width  = vertical ? 2 : 5;
                int height = vertical ? 5 : 2;

                int maxX = GRID - height;
                int maxY = GRID - width;

                int x = myRand() % (maxX + 1);
                int y = myRand() % (maxY + 1);

                if (isAreaFree(g, x, y, width, height)) {
                    for (int i = 0; i < height; i++)
                        for (int j = 0; j < width; j++)
                            g[x + i][y + j] = shipId;
                    placed = 1;
                }
            }
        }
        shipId++;
    }
}

int allDestroyed(int g[GRID][GRID]) {
    for (int i = 0; i < GRID; i++)
        for (int j = 0; j < GRID; j++)
            if (g[i][j] > 0)
                return 0;
    return 1;
}

int isShipSunk(int g[GRID][GRID], int shipId) {
    for (int i = 0; i < GRID; i++)
        for (int j = 0; j < GRID; j++)
            if (g[i][j] == shipId)
                return 0;
    return 1;
}

void markShipSunk(int g[GRID][GRID], int shipId) {
    for (int i = 0; i < GRID; i++)
        for (int j = 0; j < GRID; j++)
            if (g[i][j] == -shipId)
                g[i][j] = -200 - shipId;
}

Player* getCurrentPlayer(int mode, GameState *gs) {
    if (mode == 1)
        return (gs->currentPlayer == 0) ? &player1 : &player2;
    return &player1;
}

Player* getOpponent(int mode, GameState *gs) {
    if (mode == 1)
        return (gs->currentPlayer == 0) ? &player2 : &player1;
    return &player2;
}

void showInfoBox(SDL_Window *window, const char *format, int a, int b) {
    char buf[256];
    snprintf(buf, sizeof(buf), format, a, b);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Info", buf, window);
}


ScoreMode getScoreModeFromGameMode(int mode) {
    switch (mode) {
        case 2: return SCORE_EASY;
        case 3: return SCORE_MEDIUM;
        case 4: return SCORE_HARD;
        case 1: return SCORE_PVP;
        default: return SCORE_EASY;
    }
}


