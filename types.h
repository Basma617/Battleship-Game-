#ifndef TYPES_H
#define TYPES_H

extern unsigned int seed;
int myRand(void);
#define GRID 14
#define CELL 40
#define WIDTH (GRID * CELL * 2)
#define HEIGHT (GRID * CELL)
#define SHIP_COUNT 6
#define TURN_DELAY 500
#define PVP_DELAY 900

#define MAX_SCORES 10
#define NAME_LEN 50

typedef struct {
    int grid[GRID][GRID];
    int score;
} Player;

typedef struct {
    int sizes[SHIP_COUNT];
} ShipConfig;

typedef struct {
    int lastHitX;
    int lastHitY;
    int anchorX;
    int anchorY;
    int currentX;
    int currentY;
    int direction;
    int shots[GRID][GRID];
} AIState;


typedef struct {
    int currentPlayer;
    int waitingNextPlayer;
    int waitingDelay;
    int waitingPvPDelay;
    Uint32 lastActionTime;
    Uint32 lastPlayerActionTime;
} GameState;

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
} GraphicsContext;

typedef struct {
    char name[NAME_LEN];
    int score;
} Score;

typedef enum {
    SCORE_EASY,
    SCORE_MEDIUM,
    SCORE_HARD,
    SCORE_PVP
} ScoreMode;

extern Player player1, player2;
extern ShipConfig shipConfig;
extern AIState aiState;
extern GameState gameState;
extern GraphicsContext graphics;

extern int placementMode;

#endif






