#include <SDL3/SDL.h>
#ifndef RENDER_H
#define RENDER_H
#include "types.h"

void drawGradientRect(SDL_Renderer *renderer, int x, int y, int w, int h, 
                      int r1, int g1, int b1, int r2, int g2, int b2);
void drawFilledCircle(SDL_Renderer *renderer, int cx, int cy, int radius);
void drawCircleOutline(SDL_Renderer *renderer, int cx, int cy, int radius);
void drawWaves(SDL_Renderer *renderer, int x, int y, int w, int h, Uint32 time);
void drawExplosion(SDL_Renderer *renderer, int x, int y, int size);
void drawSmoke(SDL_Renderer *renderer, int x, int y, Uint32 time);
void drawRealisticShip(GraphicsContext *gfx, int x, int y, int size, 
                       int isVertical, int position, int length, int shipId);
void drawDamagedShip(GraphicsContext *gfx, int x, int y, int size, Uint32 time);
void drawSunkenShip(GraphicsContext *gfx, int x, int y, int size, Uint32 time);
void drawMissedShot(GraphicsContext *gfx, int x, int y, int size, Uint32 time);


void drawGrid(GraphicsContext *gfx, int g[GRID][GRID], int offsetX, int reveal);
void drawTurnScreen(GraphicsContext *gfx, int player);
void drawShipPreview(GraphicsContext *gfx, int shipSize, int vertical);
void drawShipGhost(GraphicsContext *gfx, int grid[GRID][GRID], int mouseX, int mouseY,int shipSize, int vertical);
void placeShipsSDL(GraphicsContext *gfx, Player *p, ShipConfig *config, int playerNum);





#endif