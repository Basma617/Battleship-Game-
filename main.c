#define SDL_MAIN_HANDLED
#include <stdio.h>
#include <SDL3/SDL.h>

#include "types.h"
#include "random.h"
#include "render.h"
#include "ai.h"
#include "files.h"
#include "utils.h"



int main() {
    int mode;

    printf("1 : Joueur vs Joueur\n");
    printf("2 : Joueur vs IA (Facile)\n");
    printf("3 : Joueur vs IA (Moyen)\n");
    printf("4 : Joueur vs IA (Difficile)\n");
    printf("Choix : ");
    scanf("%d", &mode);

    if (mode < 1 || mode > 4) {
        printf("Mode invalide. Fin du programme.\n");
        return 1;
    }

        // ===== DEMANDER LES NOMS DES JOUEURS ICI =====
    char player1Name[50];
    char player2Name[50];

    if (mode == 1) {
        // Mode PVP - demander les deux noms
        printf("\n=== Configuration des joueurs ===\n");
        printf("Entrez le nom du Joueur 1 : ");
        scanf("%49s", player1Name);
        printf("Entrez le nom du Joueur 2 : ");
        scanf("%49s", player2Name);
        printf("\nBienvenue %s et %s !\n\n", player1Name, player2Name);
    } else {
        // Mode contre IA - demander un seul nom
        printf("\n=== Configuration du joueur ===\n");
        printf("Entrez votre nom : ");
        scanf("%49s", player1Name);
        printf("\nBienvenue %s !\n\n", player1Name);
        strcpy(player2Name, "IA");  // Nom par défaut pour l'IA
    }

    printf("\nChoix du placement des bateaux :\n");
    printf("1 : Placement automatique\n");
    printf("2 : Placement manuel\n");
    printf("Choix : ");
    scanf("%d", &placementMode);
    if (placementMode < 1 || placementMode > 2) {
        printf("Choix invalide. Fin du programme.\n");
        return 1;
    }

    SDL_Init(SDL_INIT_VIDEO);
    seed = SDL_GetTicks();

    graphics.window = SDL_CreateWindow("Bataille Navale", WIDTH, HEIGHT, 0);
    graphics.renderer = SDL_CreateRenderer(graphics.window, NULL);
    SDL_SetWindowPosition(graphics.window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    initPlayer(&player1);
    initPlayer(&player2);
    initAIState(&aiState);
    initGameState(&gameState);

    if (placementMode == 1) {
        placeShips(player1.grid, &shipConfig);
        placeShips(player2.grid, &shipConfig);
        if (mode == 1) {
            char msg[256];
            snprintf(msg, sizeof(msg), 
                "%s (Joueur 1 - vert) VS %s (Joueur 2 - rouge)\n\n%s commence !\n\nCliquez pour continuer...",
                player1Name, player2Name, player1Name);
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Début de partie", msg, graphics.window);
        } else {
            char msg[256];
            snprintf(msg, sizeof(msg), "%s, c'est parti !\n\nCliquez pour continuer...", player1Name);
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Début de partie", msg, graphics.window);
        }
    } else {
        if (mode == 1) {
            char msg[256];
            snprintf(msg, sizeof(msg), 
                "%s,\nplacez vos bateaux avec clic gauche\nAppuyez sur V pour changer l'orientation",
                player1Name);
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Placement", msg, graphics.window);
            placeShipsSDL(&graphics, &player1, &shipConfig, 0);
            
            snprintf(msg, sizeof(msg), 
                "%s,\nplacez vos bateaux avec clic gauche\nAppuyez sur V pour changer l'orientation",
                player2Name);
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Placement", msg, graphics.window);
            placeShipsSDL(&graphics, &player2, &shipConfig, 1);
            
            snprintf(msg, sizeof(msg), 
                "%s (vert) VS %s (rouge)\n\n%s commence !\n\nCliquez pour continuer...",
                player1Name, player2Name, player1Name);
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Début de partie", msg, graphics.window);
        } else {
            char msg[256];
            snprintf(msg, sizeof(msg), 
                "%s,\nplacez vos bateaux avec clic gauche\nAppuyez sur V pour changer l'orientation",
                player1Name);
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Placement", msg, graphics.window);
            placeShipsSDL(&graphics, &player1, &shipConfig, 0);
            placeShips(player2.grid, &shipConfig);
            
            snprintf(msg, sizeof(msg), "%s, c'est parti !\n\nCliquez pour continuer...", player1Name);
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Début de partie", msg, graphics.window);
        }
    }

    int running = 1;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                running = 0;
                break;
            }
        
            if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if ((gameState.waitingPvPDelay) && mode == 1)
                    break;

                if (gameState.waitingDelay && mode != 1)
                    break;

                if (mode == 1 && gameState.waitingNextPlayer) {
                    gameState.waitingNextPlayer = 0;
                    break;
                }

                int mx = e.button.x;
                int my = e.button.y;

                if (mx < GRID * CELL)
                    break;

                int x = my / CELL;
                int y = (mx - GRID * CELL) / CELL;

                Player *target = getOpponent(mode, &gameState);
                Player *current = getCurrentPlayer(mode, &gameState);

                if (target->grid[x][y] == 0) {
                    target->grid[x][y] = -99;
                    current->score -= 1;
                }
                else if (target->grid[x][y] > 0) {
                    int shipId = target->grid[x][y];
                    target->grid[x][y] = -shipId;
                    current->score += 10; // Toucher un bateau : +10 pts
                    target->score -= 5; // Être touché : -5 pts

                    if (isShipSunk(target->grid, shipId)) {
                        markShipSunk(target->grid, shipId);
                        current->score += 20; // Couler un bateau : +20 pts
                        target->score -= 10; // Perdre un bateau : -10 pts
                    }
                }
                else {
                    break;
                }

                // VÉRIFIER LA VICTOIRE IMMÉDIATEMENT APRÈS L'ACTION
                if (mode == 1) {
                    if (allDestroyed(target->grid)) {
                        current->score += 100;
                        target->score -= 50;
                        
                        char msg[256];
                        const char *winnerName = (gameState.currentPlayer == 0) ? player1Name : player2Name;
                        
                        snprintf(msg, sizeof(msg), "🏆 VICTOIRE DE %s ! 🏆", winnerName);
                        printf("Victoire de %s\n", winnerName);
                        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Fin de partie", msg, graphics.window);
                        
                        running = 0;
                        break;
                    }
                    
                    // Pas de victoire, continuer normalement
                    gameState.lastPlayerActionTime = SDL_GetTicks();
                    gameState.waitingPvPDelay = 1;
                } else {
                    gameState.lastActionTime = SDL_GetTicks();
                    gameState.waitingDelay = 1;
                }
            }
        }

        if (gameState.waitingPvPDelay && mode == 1) {
            if (SDL_GetTicks() - gameState.lastPlayerActionTime >= PVP_DELAY) {
                gameState.currentPlayer = 1 - gameState.currentPlayer;
                gameState.waitingNextPlayer = 1;
                gameState.waitingPvPDelay = 0;
            }
        }

        if (gameState.waitingDelay && mode != 1) {
            if (SDL_GetTicks() - gameState.lastActionTime >= TURN_DELAY) {
                if (mode == 2) enemyEasy(&player1, &aiState);
                else if (mode == 3) enemyMedium(&player1, &aiState);
                else enemyHard(&player1, &aiState);

                gameState.waitingDelay = 0;
            }
        }

        // Vérification de la victoire après le tour complet (IA)

        if (mode != 1) {
            if (allDestroyed(player2.grid)) {
                player1.score += 100;
                char msg[256];
                snprintf(msg, sizeof(msg), "🏆 VICTOIRE DE %s ! 🏆\n\nVous avez vaincu l'IA !", player1Name);
                printf("Victoire de %s\n", player1Name);
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Fin de partie", msg, graphics.window);
                break;
            }
            if (allDestroyed(player1.grid)) {
                player1.score -= 50;
                char msg[256];
                snprintf(msg, sizeof(msg), "💀 DÉFAITE DE %s 💀\n\nL'IA a gagné !", player1Name);
                printf("Défaite de %s\n", player1Name);
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Fin de partie", msg, graphics.window);
                break;
            }
        }

        SDL_SetRenderDrawColor(graphics.renderer, 0, 0, 0, 255);
        SDL_RenderClear(graphics.renderer);

        if (mode == 1 && gameState.waitingNextPlayer) {
            drawTurnScreen(&graphics, gameState.currentPlayer);
        } else {
            if (mode == 1) {
                Player *current = getCurrentPlayer(mode, &gameState);
                Player *opponent = getOpponent(mode, &gameState);
                drawGrid(&graphics, current->grid, 0, 1);
                drawGrid(&graphics, opponent->grid, GRID * CELL, 0);
            } else {
                drawGrid(&graphics, player1.grid, 0, 1);
                drawGrid(&graphics, player2.grid, GRID * CELL, 0);
            }
        }

        SDL_SetRenderDrawColor(graphics.renderer, 200, 200, 200, 255);
        SDL_RenderLine(graphics.renderer, GRID * CELL - 1, 0, GRID * CELL - 1, HEIGHT);
        SDL_RenderLine(graphics.renderer, GRID * CELL, 0, GRID * CELL, HEIGHT);

        SDL_RenderPresent(graphics.renderer);
        SDL_Delay(16);
    }

    if (mode != 1) {
        ScoreMode scoreMode = getScoreModeFromGameMode(mode);
        int bestScore = getBestScore("scores.txt", scoreMode);
        
        printf("Score final de %s : %d\n", player1Name, player1.score);
        printf("Meilleur score : %d\n", bestScore);
        
        char msg[512];
        if (player1.score > bestScore) {
            snprintf(msg, sizeof(msg), 
                "🎉 NOUVEAU RECORD DE %s ! 🎉\n\nVotre score : %d\nAncien record : %d\n\nFélicitations !", 
                player1Name, player1.score, bestScore);
        } else {
            snprintf(msg, sizeof(msg), 
                "Score final de %s : %d\n\nMeilleur score : %d", 
                player1Name, player1.score, bestScore);
        }
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Résultats", msg, graphics.window);
    } else {
        ScoreMode scoreMode = getScoreModeFromGameMode(mode);
        int bestScore = getBestScore("scores.txt", scoreMode);
        
        printf("Score final de %s : %d\n", player1Name, player1.score);
        printf("Score final de %s : %d\n", player2Name, player2.score);
        printf("Meilleur score : %d\n", bestScore);
        
        char msg[512];
        int maxScore = (player1.score > player2.score) ? player1.score : player2.score;
        
        if (maxScore > bestScore) {
            const char *recordHolder = (player1.score > player2.score) ? player1Name : player2Name;
            snprintf(msg, sizeof(msg), 
                "🎉 NOUVEAU RECORD DE %s ! 🎉\n\n%s : %d\n%s : %d\n\nAncien record : %d\n\nFélicitations !", 
                recordHolder, player1Name, player1.score, player2Name, player2.score, bestScore);
        } else {
            snprintf(msg, sizeof(msg), 
                "Scores finaux :\n\n%s : %d\n%s : %d\n\nMeilleur score : %d", 
                player1Name, player1.score, player2Name, player2.score, bestScore);
        }
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Résultats", msg, graphics.window);
    }

    /* _______ Section d'enregistrement des scores __________ */

    if (mode == 1) {
        // En mode PVP, utiliser les noms déjà saisis
        ScoreMode scoreMode = getScoreModeFromGameMode(mode);
        updateTopScoresOneFile("scores.txt", scoreMode, player1Name, player1.score);
        updateTopScoresOneFile("scores.txt", scoreMode, player2Name, player2.score);
        
        printf("\nScores enregistrés pour %s et %s !\n", player1Name, player2Name);
    } else {
        // En mode contre IA, utiliser le nom déjà saisi
        ScoreMode scoreMode = getScoreModeFromGameMode(mode);
        updateTopScoresOneFile("scores.txt", scoreMode, player1Name, player1.score);
        
        printf("\nScore enregistré pour %s !\n", player1Name);
    }
    

    SDL_DestroyRenderer(graphics.renderer);
    SDL_DestroyWindow(graphics.window);
    SDL_Quit();
    return 0;
}

