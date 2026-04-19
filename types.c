#include <SDL3/SDL.h>


#include "types.h"

/* ---------------- VARIABLES GLOBALES ---------------- */


Player player1, player2;
ShipConfig shipConfig = {
    .sizes = {3, 4, 4, 5, 6, -10}
};
AIState aiState;
GameState gameState;
GraphicsContext graphics;
unsigned int seed = 98765;
int placementMode;

char player1Name[NAME_LEN] = "";
char player2Name[NAME_LEN] = "";