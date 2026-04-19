#include <SDL3/SDL.h>
#include "types.h"
#include "utils.h"
#include "ai.h"
/*---------------------------------------------------------*/
/*AI FACILE L’IA tire au hasard, sans mémoriser ni exploiter les coups réussis.*/



/*Cette fonction gère le comportement de l’IA en mode facile.*/
/*Player *target
→ Le joueur ciblé (grille du joueur humain)

AIState *ai
→ État interne de l’IA (cases déjà tirées)*/

void enemyEasy(Player *target, AIState *ai) {
    int x, y;
    do { /*x = myRand() % GRID;
        y = myRand() % GRID;
        */
        x = myRand() % GRID;
        y = myRand() % GRID;
    } while (ai->shots[x][y]); /*while (ai->shots[x][y]);
*/

    ai->shots[x][y] = 1; /*ai->shots[x][y] = 1;
*/

    if (target->grid[x][y] > 0)/*if (target->grid[x][y] > 0)
*/ {    /*Récupère l’identifiant du bateau

Marque la case comme touchée (valeur négative)

Vérifie si le bateau est complètement coulé

Si oui, marque tout le bateau comme coulé*/
        int shipId = target->grid[x][y];
        target->grid[x][y] = -shipId;

        if (isShipSunk(target->grid, shipId))
            markShipSunk(target->grid, shipId);
    }
    else if (target->grid[x][y] == 0) /*target->grid[x][y] = -99;
*/
        target->grid[x][y] = -99;
}



/*-------------------------------------------------------------------*/
                                        /*AI MEDIUM   IA explore autour d’un coup réussi, mais sans déterminer l’orientation du bateau.*/
void enemyMedium(Player *target, AIState *ai) {
    int x, y;

    if (ai->lastHitX != -1)/*if (ai->lastHitX != -1)
*/ { /*Tentative autour de la dernière touche*/
        int dx[4] = {-1, 1, 0, 0};
        int dy[4] = {0, 0, -1, 1};

        for (int i = 0; i < 4; i++) {
            x = ai->lastHitX + dx[i];
            y = ai->lastHitY + dy[i];

            if (x >= 0 && x < GRID && y >= 0 && y < GRID && ai->shots[x][y] == 0) {
                ai->shots[x][y] = 1;

                if (target->grid[x][y] > 0) {
                    int shipId = target->grid[x][y];
                    target->grid[x][y] = -shipId;
                    ai->lastHitX = x;
                    ai->lastHitY = y;

                    if (isShipSunk(target->grid, shipId)) {
                        markShipSunk(target->grid, shipId);
                        ai->lastHitX = -1;
                        ai->lastHitY = -1;
                    }
                } else {
                    target->grid[x][y] = -99;/*Marque la case comme ratée (-99)*/
                }
                return;
            }
        }

        ai->lastHitX = -1;
        ai->lastHitY = -1;
    }

    do {
        x = myRand() % GRID;
        y = myRand() % GRID;
    } while (ai->shots[x][y]);

    ai->shots[x][y] = 1;

    if (target->grid[x][y] > 0) {
        int shipId = target->grid[x][y];
        target->grid[x][y] = -shipId;
        ai->lastHitX = x;
        ai->lastHitY = y;

        if (isShipSunk(target->grid, shipId)) {
            markShipSunk(target->grid, shipId);
            ai->lastHitX = -1;
            ai->lastHitY = -1;
        }
    } else {
        target->grid[x][y] = -99;
    }
}


/*---------------------------------------------------------*/
                                        /*AI HARD  */

void enemyHard(Player *target, AIState *ai) {
    int dx[5] = {0, -1, 1, 0, 0};
    int dy[5] = {0, 0, 0, -1, 1};
    int x, y;

    if (ai->direction != 0) {/*Continue à tirer dans une direction déjà trouvée

Si touché → continue

Si raté → revient au point d’ancrage et annule la direction*/
        x = ai->currentX + dx[ai->direction];
        y = ai->currentY + dy[ai->direction];

        if (x >= 0 && x < GRID && y >= 0 && y < GRID && ai->shots[x][y] == 0) {
            ai->shots[x][y] = 1;

            if (target->grid[x][y] > 0) {
                int shipId = target->grid[x][y];
                target->grid[x][y] = -shipId;
                ai->currentX = x;
                ai->currentY = y;

                if (isShipSunk(target->grid, shipId)) {
                    markShipSunk(target->grid, shipId);
                    ai->anchorX = ai->anchorY = -1;
                    ai->currentX = ai->currentY = -1;
                    ai->direction = 0;
                }
            } else {
                target->grid[x][y] = -99;
                ai->currentX = ai->anchorX;
                ai->currentY = ai->anchorY;
                ai->direction = 0;
            }
            return;
        } else {
            ai->currentX = ai->anchorX;
            ai->currentY = ai->anchorY;
            ai->direction = 0;
        }
    }

    if (ai->anchorX != -1)/*on a le tir Exploration autour du point d’ancrage */ 
    /*Test des 4 directions autour du premier impact

Dès qu’un tir touche :

Définit une direction

Continue dans cette direction

Si le bateau est coulé :

Réinitialise toutes les variables de suivi*/
    {
        int shotMade = 0;

        for (int dir = 1; dir <= 4 && !shotMade; dir++) {
            x = ai->anchorX + dx[dir];
            y = ai->anchorY + dy[dir];

            if (x >= 0 && x < GRID && y >= 0 && y < GRID && ai->shots[x][y] == 0) {
                ai->shots[x][y] = 1;

                if (target->grid[x][y] > 0) {
                    int shipId = target->grid[x][y];
                    target->grid[x][y] = -shipId;
                    ai->direction = dir;
                    ai->currentX = x;
                    ai->currentY = y;

                    if (isShipSunk(target->grid, shipId)) {
                        markShipSunk(target->grid, shipId);
                        ai->anchorX = ai->anchorY = -1;
                        ai->currentX = ai->currentY = -1;
                        ai->direction = 0;
                    }
                } else {
                    target->grid[x][y] = -99;
                }

                shotMade = 1;
            }
        }

        if (!shotMade) {
            for (int i = 0; i < GRID && !shotMade; i++) {
                for (int j = 0; j < GRID && !shotMade; j++) {
                    if (target->grid[i][j] < 0 && target->grid[i][j] > -99) {
                        for (int dir = 1; dir <= 4 && !shotMade; dir++) {
                            x = i + dx[dir];
                            y = j + dy[dir];
                            if (x >= 0 && x < GRID && y >= 0 && y < GRID && ai->shots[x][y] == 0) {
                                ai->shots[x][y] = 1;

                                if (target->grid[x][y] > 0) {
                                    int shipId = target->grid[x][y];
                                    target->grid[x][y] = -shipId;
                                    ai->anchorX = i;
                                    ai->anchorY = j;
                                    ai->currentX = x;
                                    ai->currentY = y;
                                    ai->direction = dir;

                                    if (isShipSunk(target->grid, shipId)) {
                                        markShipSunk(target->grid, shipId);
                                        ai->anchorX = ai->anchorY = -1;
                                        ai->currentX = ai->currentY = -1;
                                        ai->direction = 0;
                                    }
                                } else {
                                    target->grid[x][y] = -99;
                                }

                                shotMade = 1;
                            }
                        }
                    }
                }
            }
        }

        if (shotMade) return;
    }

    do {
        x = myRand() % GRID;
        y = myRand() % GRID;
    } while (ai->shots[x][y] || (x + y) % 2 != 0);

    ai->shots[x][y] = 1;

    if (target->grid[x][y] > 0) {
        int shipId = target->grid[x][y];
        target->grid[x][y] = -shipId;
        ai->anchorX = x;
        ai->anchorY = y;
        ai->currentX = x;
        ai->currentY = y;
    } else {
        target->grid[x][y] = -99;
    }
}