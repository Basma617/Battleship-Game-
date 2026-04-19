#include <SDL3/SDL.h>
#include "types.h"
#include "utils.h"
#include "render.h"




// Dessiner un gradient
void drawGradientRect(SDL_Renderer *renderer, int x, int y, int w, int h, 
                      int r1, int g1, int b1, int r2, int g2, int b2) {
    for (int i = 0; i < h; i++) {
        float ratio = (float)i / h;
        int r = r1 + (int)((r2 - r1) * ratio);
        int g = g1 + (int)((g2 - g1) * ratio);
        int b = b1 + (int)((b2 - b1) * ratio);
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderLine(renderer, x, y + i, x + w, y + i);
    }
}

// Dessiner un cercle rempli (algorithme de Bresenham adapté)
void drawFilledCircle(SDL_Renderer *renderer, int cx, int cy, int radius) {
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x*x + y*y <= radius*radius) {
                SDL_RenderPoint(renderer, cx + x, cy + y);
            }
        }
    }
}

// Dessiner un cercle non rempli
void drawCircleOutline(SDL_Renderer *renderer, int cx, int cy, int radius) {
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y) {
        SDL_RenderPoint(renderer, cx + x, cy + y);
        SDL_RenderPoint(renderer, cx + y, cy + x);
        SDL_RenderPoint(renderer, cx - y, cy + x);
        SDL_RenderPoint(renderer, cx - x, cy + y);
        SDL_RenderPoint(renderer, cx - x, cy - y);
        SDL_RenderPoint(renderer, cx - y, cy - x);
        SDL_RenderPoint(renderer, cx + y, cy - x);
        SDL_RenderPoint(renderer, cx + x, cy - y);

        if (err <= 0) {
            y += 1;
            err += 2*y + 1;
        }
        if (err > 0) {
            x -= 1;
            err -= 2*x + 1;
        }
    }
}

// Dessiner des vagues animées
void drawWaves(SDL_Renderer *renderer, int x, int y, int w, int h, Uint32 time) {
    // Fond océan avec gradient
    drawGradientRect(renderer, x, y, w, h, 0, 100, 160, 0, 140, 200);
    
    // Vagues animées (forme sinusoïdale approximée)
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    int offset = (time / 100) % 20;
    
    for (int i = 0; i < 3; i++) {
        int waveY = y + h/4 + i * h/4 + offset - 10;
        SDL_SetRenderDrawColor(renderer, 40, 160, 220, 80);
        SDL_FRect wave = { x + 3, waveY, w - 6, 2 };
        SDL_RenderFillRect(renderer, &wave);
    }
    
    // Reflets lumineux
    SDL_SetRenderDrawColor(renderer, 100, 200, 255, 60);
    SDL_FRect shine = { x + w/4, y + h/3, w/2, 3 };
    SDL_RenderFillRect(renderer, &shine);
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

// Dessiner explosion/impact
void drawExplosion(SDL_Renderer *renderer, int x, int y, int size) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    
    // Cercle rouge central
    SDL_SetRenderDrawColor(renderer, 255, 50, 0, 200);
    drawFilledCircle(renderer, x, y, size);
    
    // Cercle orange intermédiaire
    SDL_SetRenderDrawColor(renderer, 255, 120, 0, 150);
    drawFilledCircle(renderer, x, y, size + 3);
    
    // Cercle jaune externe
    SDL_SetRenderDrawColor(renderer, 255, 200, 0, 100);
    drawFilledCircle(renderer, x, y, size + 6);
    
    // Particules de feu (8 directions)
    int directions[8][2] = {
        {1, 0}, {1, 1}, {0, 1}, {-1, 1},
        {-1, 0}, {-1, -1}, {0, -1}, {1, -1}
    };
    
    for (int i = 0; i < 8; i++) {
        int px = x + (size + 8) * directions[i][0];
        int py = y + (size + 8) * directions[i][1];
        SDL_SetRenderDrawColor(renderer, 255, 150, 0, 180);
        SDL_FRect particle = { px - 2, py - 2, 4, 4 };
        SDL_RenderFillRect(renderer, &particle);
    }
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

// Dessiner fumée
void drawSmoke(SDL_Renderer *renderer, int x, int y, Uint32 time) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    
    int offset = (time / 50) % 15;
    
    for (int i = 0; i < 3; i++) {
        int smokeY = y - offset - i * 8;
        int smokeSize = 6 + i * 2;
        SDL_SetRenderDrawColor(renderer, 80, 80, 80, 120 - i * 30);
        drawFilledCircle(renderer, x, smokeY, smokeSize);
    }
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

// Dessiner un bateau ultra-réaliste
void drawRealisticShip(GraphicsContext *gfx, int x, int y, int size, 
                       int isVertical, int position, int length, int shipId) {
    SDL_FRect cell = { x, y, size - 1, size - 1 };
    
    // Gradient métallique pour la coque
    if (isVertical) {
        drawGradientRect(gfx->renderer, x, y, size - 1, size - 1, 
                        60, 70, 80, 90, 100, 110);
    } else {
        drawGradientRect(gfx->renderer, x, y, size - 1, size - 1,
                        90, 100, 110, 60, 70, 80);
    }
    
    // Bordure de la coque
    SDL_SetRenderDrawColor(gfx->renderer, 40, 50, 60, 255);
    SDL_RenderRect(gfx->renderer, &cell);
    
    // Effet 3D - ombres et lumières
    SDL_SetRenderDrawBlendMode(gfx->renderer, SDL_BLENDMODE_BLEND);
    
    // Ombre en bas/droite
    SDL_SetRenderDrawColor(gfx->renderer, 0, 0, 0, 80);
    SDL_RenderLine(gfx->renderer, x, y + size - 2, x + size - 1, y + size - 2);
    SDL_RenderLine(gfx->renderer, x + size - 2, y, x + size - 2, y + size - 1);
    
    // Lumière en haut/gauche
    SDL_SetRenderDrawColor(gfx->renderer, 255, 255, 255, 60);
    SDL_RenderLine(gfx->renderer, x + 1, y + 1, x + size - 2, y + 1);
    SDL_RenderLine(gfx->renderer, x + 1, y + 1, x + 1, y + size - 2);
    
    SDL_SetRenderDrawBlendMode(gfx->renderer, SDL_BLENDMODE_NONE);
    
    // Détails selon la position
    if (isVertical) {
        // PROUE (avant)
        if (position == 0) {
            // Forme pointue
            SDL_SetRenderDrawColor(gfx->renderer, 100, 110, 120, 255);
            for (int i = 0; i < size/3; i++) {
                SDL_RenderLine(gfx->renderer, 
                              x + size/2 - i/2, y + i,
                              x + size/2 + i/2, y + i);
            }
            // Ancre
            SDL_SetRenderDrawColor(gfx->renderer, 180, 150, 100, 255);
            SDL_FRect anchor = { x + size/2 - 3, y + size/4, 6, 8 };
            SDL_RenderFillRect(gfx->renderer, &anchor);
            SDL_FRect anchorBottom = { x + size/2 - 5, y + size/4 + 6, 10, 3 };
            SDL_RenderFillRect(gfx->renderer, &anchorBottom);
        }
        // POUPE (arrière)
        else if (position == length - 1) {
            // Moteur/hélice
            SDL_SetRenderDrawColor(gfx->renderer, 50, 50, 50, 255);
            SDL_FRect engine = { x + size/3, y + size - size/3, size/3, size/4 };
            SDL_RenderFillRect(gfx->renderer, &engine);
            
            // Gouvernail
            SDL_SetRenderDrawColor(gfx->renderer, 70, 70, 70, 255);
            SDL_FRect rudder = { x + size/2 - 2, y + size - size/4, 4, size/5 };
            SDL_RenderFillRect(gfx->renderer, &rudder);
            
            // Hélices (représentées par des petits rectangles)
            SDL_SetRenderDrawColor(gfx->renderer, 90, 90, 90, 255);
            SDL_FRect prop1 = { x + size/3 - 2, y + size - size/4, 2, 6 };
            SDL_FRect prop2 = { x + 2*size/3, y + size - size/4, 2, 6 };
            SDL_RenderFillRect(gfx->renderer, &prop1);
            SDL_RenderFillRect(gfx->renderer, &prop2);
        }
        // MILIEU
        else {
            // Tour de contrôle
            if (position == length / 2) {
                SDL_SetRenderDrawColor(gfx->renderer, 80, 90, 100, 255);
                SDL_FRect tower = { x + size/3, y + size/4, size/3, size/2 };
                SDL_RenderFillRect(gfx->renderer, &tower);
                
                // Bordure de la tour
                SDL_SetRenderDrawColor(gfx->renderer, 100, 110, 120, 255);
                SDL_RenderRect(gfx->renderer, &tower);
                
                // Radar/antenne
                SDL_SetRenderDrawColor(gfx->renderer, 120, 130, 140, 255);
                SDL_RenderLine(gfx->renderer, x + size/2, y + size/4, x + size/2, y + 2);
                drawFilledCircle(gfx->renderer, x + size/2, y + 3, 3);
            }
            
            // Hublots illuminés
            int numWindows = 2;
            for (int w = 0; w < numWindows; w++) {
                SDL_FRect window = { 
                    x + size/4 + w * (size/2), 
                    y + size/2 - 3, 
                    5, 5 
                };
                SDL_SetRenderDrawColor(gfx->renderer, 255, 240, 180, 255);
                SDL_RenderFillRect(gfx->renderer, &window);
                
                // Bordure de fenêtre
                SDL_SetRenderDrawColor(gfx->renderer, 100, 100, 100, 255);
                SDL_RenderRect(gfx->renderer, &window);
                
                // Reflet dans la fenêtre
                SDL_SetRenderDrawBlendMode(gfx->renderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(gfx->renderer, 255, 255, 255, 100);
                SDL_FRect reflection = { window.x + 1, window.y + 1, 2, 2 };
                SDL_RenderFillRect(gfx->renderer, &reflection);
                SDL_SetRenderDrawBlendMode(gfx->renderer, SDL_BLENDMODE_NONE);
            }
            
            // Canons/tourelles (pour grands bateaux)
            if (shipId >= 4) {
                SDL_SetRenderDrawColor(gfx->renderer, 60, 60, 60, 255);
                SDL_FRect cannon1 = { x + 3, y + size/3, 4, size/4 };
                SDL_FRect cannon2 = { x + size - 7, y + size/3, 4, size/4 };
                SDL_RenderFillRect(gfx->renderer, &cannon1);
                SDL_RenderFillRect(gfx->renderer, &cannon2);
                
                // Barils des canons
                SDL_SetRenderDrawColor(gfx->renderer, 40, 40, 40, 255);
                SDL_FRect barrel1 = { x - 2, y + size/3 + 2, 5, 3 };
                SDL_FRect barrel2 = { x + size - 3, y + size/3 + 2, 5, 3 };
                SDL_RenderFillRect(gfx->renderer, &barrel1);
                SDL_RenderFillRect(gfx->renderer, &barrel2);
            }
            
            // Pont (deck)
            SDL_SetRenderDrawBlendMode(gfx->renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(gfx->renderer, 100, 80, 60, 100);
            SDL_FRect deck = { x + 5, y + size/2 + 5, size - 10, 2 };
            SDL_RenderFillRect(gfx->renderer, &deck);
            SDL_SetRenderDrawBlendMode(gfx->renderer, SDL_BLENDMODE_NONE);
        }
    } else {
        // HORIZONTAL - mêmes principes adaptés
        if (position == 0) {
            SDL_SetRenderDrawColor(gfx->renderer, 100, 110, 120, 255);
            for (int i = 0; i < size/3; i++) {
                SDL_RenderLine(gfx->renderer, 
                              x + i, y + size/2 - i/2,
                              x + i, y + size/2 + i/2);
            }
            SDL_SetRenderDrawColor(gfx->renderer, 180, 150, 100, 255);
            SDL_FRect anchor = { x + size/4, y + size/2 - 3, 8, 6 };
            SDL_RenderFillRect(gfx->renderer, &anchor);
            SDL_FRect anchorBottom = { x + size/4 + 6, y + size/2 - 5, 3, 10 };
            SDL_RenderFillRect(gfx->renderer, &anchorBottom);
        }
        else if (position == length - 1) {
            SDL_SetRenderDrawColor(gfx->renderer, 50, 50, 50, 255);
            SDL_FRect engine = { x + size - size/3, y + size/3, size/4, size/3 };
            SDL_RenderFillRect(gfx->renderer, &engine);
            
            SDL_SetRenderDrawColor(gfx->renderer, 90, 90, 90, 255);
            SDL_FRect prop1 = { x + size - size/4, y + size/3 - 2, 6, 2 };
            SDL_FRect prop2 = { x + size - size/4, y + 2*size/3, 6, 2 };
            SDL_RenderFillRect(gfx->renderer, &prop1);
            SDL_RenderFillRect(gfx->renderer, &prop2);
        }
        else {
            if (position == length / 2) {
                SDL_SetRenderDrawColor(gfx->renderer, 80, 90, 100, 255);
                SDL_FRect tower = { x + size/4, y + size/3, size/2, size/3 };
                SDL_RenderFillRect(gfx->renderer, &tower);
                SDL_SetRenderDrawColor(gfx->renderer, 100, 110, 120, 255);
                SDL_RenderRect(gfx->renderer, &tower);
                
                SDL_SetRenderDrawColor(gfx->renderer, 120, 130, 140, 255);
                SDL_RenderLine(gfx->renderer, x + size/2, y + size/3, x + size/2, y + 2);
                drawFilledCircle(gfx->renderer, x + size/2, y + 3, 3);
            }
            
            for (int w = 0; w < 2; w++) {
                SDL_FRect window = { 
                    x + size/2 - 3, 
                    y + size/4 + w * (size/2), 
                    5, 5 
                };
                SDL_SetRenderDrawColor(gfx->renderer, 255, 240, 180, 255);
                SDL_RenderFillRect(gfx->renderer, &window);
                SDL_SetRenderDrawColor(gfx->renderer, 100, 100, 100, 255);
                SDL_RenderRect(gfx->renderer, &window);
                
                SDL_SetRenderDrawBlendMode(gfx->renderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(gfx->renderer, 255, 255, 255, 100);
                SDL_FRect reflection = { window.x + 1, window.y + 1, 2, 2 };
                SDL_RenderFillRect(gfx->renderer, &reflection);
                SDL_SetRenderDrawBlendMode(gfx->renderer, SDL_BLENDMODE_NONE);
            }
            
            if (shipId >= 4) {
                SDL_SetRenderDrawColor(gfx->renderer, 60, 60, 60, 255);
                SDL_FRect cannon1 = { x + size/3, y + 3, size/4, 4 };
                SDL_FRect cannon2 = { x + size/3, y + size - 7, size/4, 4 };
                SDL_RenderFillRect(gfx->renderer, &cannon1);
                SDL_RenderFillRect(gfx->renderer, &cannon2);
                
                SDL_SetRenderDrawColor(gfx->renderer, 40, 40, 40, 255);
                SDL_FRect barrel1 = { x + size/3 + 2, y - 2, 3, 5 };
                SDL_FRect barrel2 = { x + size/3 + 2, y + size - 3, 3, 5 };
                SDL_RenderFillRect(gfx->renderer, &barrel1);
                SDL_RenderFillRect(gfx->renderer, &barrel2);
            }
            
            SDL_SetRenderDrawBlendMode(gfx->renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(gfx->renderer, 100, 80, 60, 100);
            SDL_FRect deck = { x + size/2 + 5, y + 5, 2, size - 10 };
            SDL_RenderFillRect(gfx->renderer, &deck);
            SDL_SetRenderDrawBlendMode(gfx->renderer, SDL_BLENDMODE_NONE);
        }
    }
    
    // Numéro du bateau
    SDL_SetRenderDrawColor(gfx->renderer, 200, 200, 200, 255);
    SDL_FRect numBox = { x + size - 12, y + 2, 8, 8 };
    SDL_RenderRect(gfx->renderer, &numBox);
    
    // Lignes de rivetage sur la coque
    SDL_SetRenderDrawBlendMode(gfx->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(gfx->renderer, 40, 40, 50, 150);
    for (int r = 0; r < 3; r++) {
        if (isVertical) {
            int lineY = y + size/4 + r * size/4;
            SDL_RenderLine(gfx->renderer, x + 3, lineY, x + size - 4, lineY);
        } else {
            int lineX = x + size/4 + r * size/4;
            SDL_RenderLine(gfx->renderer, lineX, y + 3, lineX, y + size - 4);
        }
    }
    SDL_SetRenderDrawBlendMode(gfx->renderer, SDL_BLENDMODE_NONE);
}

// Dessiner bateau endommagé avec effets réalistes
void drawDamagedShip(GraphicsContext *gfx, int x, int y, int size, Uint32 time) {
    SDL_FRect cell = { x, y, size - 1, size - 1 };
    
    // Coque endommagée avec gradient
    drawGradientRect(gfx->renderer, x, y, size - 1, size - 1,
                    150, 40, 30, 100, 20, 10);
    
    // Bordure déchirée
    SDL_SetRenderDrawColor(gfx->renderer, 80, 20, 10, 255);
    SDL_RenderRect(gfx->renderer, &cell);
    
    // Trous de balles/impacts
    SDL_SetRenderDrawColor(gfx->renderer, 0, 0, 0, 255);
    drawFilledCircle(gfx->renderer, x + size/3, y + size/3, 3);
    drawFilledCircle(gfx->renderer, x + 2*size/3, y + 2*size/3, 3);
    drawFilledCircle(gfx->renderer, x + size/2, y + size - size/4, 2);
    
    // Explosion active
    drawExplosion(gfx->renderer, x + size/2, y + size/2, 8);
    
    // Fumée s'échappant
    drawSmoke(gfx->renderer, x + size/2, y + size/4, time);
    
    // Flammes animées
    SDL_SetRenderDrawBlendMode(gfx->renderer, SDL_BLENDMODE_BLEND);
    int flicker = (time / 100) % 3;
    SDL_SetRenderDrawColor(gfx->renderer, 255, 100 + flicker * 30, 0, 200);
    
    // Flammes principales
    SDL_FRect fire1 = { x + size/4, y + size/3, 8, 12 };
    SDL_FRect fire2 = { x + size - size/3, y + size/2, 6, 10 };
    SDL_RenderFillRect(gfx->renderer, &fire1);
    SDL_RenderFillRect(gfx->renderer, &fire2);
    
    // Pointes de flammes
    SDL_SetRenderDrawColor(gfx->renderer, 255, 200, 0, 180);
    SDL_FRect fire1Top = { x + size/4 + 2, y + size/3 - 4, 4, 5 };
    SDL_FRect fire2Top = { x + size - size/3 + 1, y + size/2 - 3, 4, 4 };
    SDL_RenderFillRect(gfx->renderer, &fire1Top);
    SDL_RenderFillRect(gfx->renderer, &fire2Top);
    
    SDL_SetRenderDrawBlendMode(gfx->renderer, SDL_BLENDMODE_NONE);
    
    // Croix rouge de dégâts
    SDL_SetRenderDrawColor(gfx->renderer, 255, 0, 0, 255);
    for (int i = 0; i < 3; i++) {
        SDL_RenderLine(gfx->renderer, x + 5 + i, y + 5, x + size - 6 + i, y + size - 6);
        SDL_RenderLine(gfx->renderer, x + size - 6 - i, y + 5, x + 5 - i, y + size - 6);
    }
    
    // Étincelles
    SDL_SetRenderDrawBlendMode(gfx->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(gfx->renderer, 255, 255, 100, 200);
    int sparkOffset = (time / 80) % 4;
    SDL_RenderPoint(gfx->renderer, x + size/3 + sparkOffset, y + size/4);
    SDL_RenderPoint(gfx->renderer, x + 2*size/3 - sparkOffset, y + 3*size/4);
    SDL_SetRenderDrawBlendMode(gfx->renderer, SDL_BLENDMODE_NONE);
}

// Dessiner bateau coulé avec effet de naufrage
void drawSunkenShip(GraphicsContext *gfx, int x, int y, int size, Uint32 time) {
    SDL_FRect cell = { x, y, size - 1, size - 1 };
    
    // Fond océan sombre
    drawGradientRect(gfx->renderer, x, y, size - 1, size - 1,
                    10, 30, 50, 5, 15, 25);
    
    // Épave visible sous l'eau
    SDL_SetRenderDrawBlendMode(gfx->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(gfx->renderer, 40, 40, 50, 150);
    SDL_FRect wreck = { x + 5, y + 5, size - 11, size - 11 };
    SDL_RenderFillRect(gfx->renderer, &wreck);
    
    // Détails de l'épave
    SDL_SetRenderDrawColor(gfx->renderer, 30, 30, 40, 120);
    SDL_FRect wreckDetail1 = { x + 7, y + 7, size - 15, 3 };
    SDL_FRect wreckDetail2 = { x + 7, y + size - 10, size - 15, 3 };
    SDL_RenderFillRect(gfx->renderer, &wreckDetail1);
    SDL_RenderFillRect(gfx->renderer, &wreckDetail2);
    
    // Bulles d'air remontant
    int bubbleOffset = (time / 200) % (size - 10);
    for (int i = 0; i < 3; i++) {
        int by = y + size - bubbleOffset - i * (size / 4);
        if (by > y && by < y + size) {
            SDL_SetRenderDrawColor(gfx->renderer, 150, 200, 255, 100);
            drawFilledCircle(gfx->renderer, x + size/2 + i * 5 - 5, by, 2 + i);
            
            // Contour des bulles
            SDL_SetRenderDrawColor(gfx->renderer, 200, 230, 255, 150);
            drawCircleOutline(gfx->renderer, x + size/2 + i * 5 - 5, by, 2 + i);
        }
    }
    
    SDL_SetRenderDrawBlendMode(gfx->renderer, SDL_BLENDMODE_NONE);
    
    // X noir épais pour marquer le naufrage
    SDL_SetRenderDrawColor(gfx->renderer, 0, 0, 0, 255);
    for (int i = 0; i < 4; i++) {
        SDL_RenderLine(gfx->renderer, x + 3 + i, y + 3, x + size - 4 + i, y + size - 4);
        SDL_RenderLine(gfx->renderer, x + size - 4 - i, y + 3, x + 3 - i, y + size - 4);
    }
    
    // Contour blanc pour contraste
    SDL_SetRenderDrawColor(gfx->renderer, 200, 200, 200, 255);
    for (int i = 0; i < 2; i++) {
        SDL_RenderLine(gfx->renderer, x + 2 + i, y + 2, x + size - 3 + i, y + size - 3);
        SDL_RenderLine(gfx->renderer, x + size - 3 - i, y + 2, x + 2 - i, y + size - 3);
    }
    
    // Effet d'huile à la surface
    SDL_SetRenderDrawBlendMode(gfx->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(gfx->renderer, 80, 60, 40, 80);
    SDL_FRect oil = { x + 2, y + 2, size - 4, 4 };
    SDL_RenderFillRect(gfx->renderer, &oil);
    
    // Reflets arc-en-ciel de l'huile
    int oilShimmer = (time / 150) % 3;
    if (oilShimmer == 0) {
        SDL_SetRenderDrawColor(gfx->renderer, 150, 100, 200, 60);
    } else if (oilShimmer == 1) {
        SDL_SetRenderDrawColor(gfx->renderer, 100, 150, 200, 60);
    } else {
        SDL_SetRenderDrawColor(gfx->renderer, 200, 150, 100, 60);
    }
    SDL_FRect oilShine = { x + 3, y + 3, size - 6, 2 };
    SDL_RenderFillRect(gfx->renderer, &oilShine);
    
    SDL_SetRenderDrawBlendMode(gfx->renderer, SDL_BLENDMODE_NONE);
}

// Dessiner tir manqué avec splash réaliste
void drawMissedShot(GraphicsContext *gfx, int x, int y, int size, Uint32 time) {
    // Eau avec vagues
    drawWaves(gfx->renderer, x, y, size - 1, size - 1, time);
    
    // Impact/splash
    SDL_SetRenderDrawBlendMode(gfx->renderer, SDL_BLENDMODE_BLEND);
    
    // Cercles d'eau concentriques
    int ripple = (time / 100) % 10;
    for (int i = 0; i < 3; i++) {
        int radius = 5 + i * 4 + ripple;
        if (radius < size/2) {
            SDL_SetRenderDrawColor(gfx->renderer, 255, 255, 255, 150 - i * 40);
            drawCircleOutline(gfx->renderer, x + size/2, y + size/2, radius);
            // Épaissir le cercle
            drawCircleOutline(gfx->renderer, x + size/2, y + size/2, radius + 1);
        }
    }
    
    // Éclaboussures (8 directions principales)
    int splashPositions[8][2] = {
        {12, 0}, {8, 8}, {0, 12}, {-8, 8},
        {-12, 0}, {-8, -8}, {0, -12}, {8, -8}
    };
    
    SDL_SetRenderDrawColor(gfx->renderer, 200, 230, 255, 180);
    for (int i = 0; i < 8; i++) {
        int sx = x + size/2 + splashPositions[i][0];
        int sy = y + size/2 + splashPositions[i][1];
        SDL_FRect splash = { sx - 2, sy - 2, 4, 4 };
        SDL_RenderFillRect(gfx->renderer, &splash);
    }
    
    // Point d'impact central
    SDL_SetRenderDrawColor(gfx->renderer, 255, 255, 255, 220);
    drawFilledCircle(gfx->renderer, x + size/2, y + size/2, 3);
    
    SDL_SetRenderDrawBlendMode(gfx->renderer, SDL_BLENDMODE_NONE);
}


void drawGrid(GraphicsContext *gfx, int g[GRID][GRID], int offsetX, int reveal) {
    Uint32 currentTime = SDL_GetTicks();
    
    for (int i = 0; i < GRID; i++) {
        for (int j = 0; j < GRID; j++) {
            int x = offsetX + j * CELL;
            int y = i * CELL;
            int cellValue = g[i][j];

            if (cellValue <= -200) {
                // Bateau coulé - effet de naufrage réaliste
                drawSunkenShip(gfx, x, y, CELL, currentTime);
            }
            else if (cellValue < 0 && cellValue != -99) {
                // Bateau touché - en feu avec animations
                drawDamagedShip(gfx, x, y, CELL, currentTime);
            }
            else if (cellValue == -99) {
                // Tir manqué - splash animé dans l'eau
                drawMissedShot(gfx, x, y, CELL, currentTime);
            }
            else if (cellValue > 0 && reveal) {
                // Bateau intact - ultra-détaillé
                int shipId = cellValue;
                int position = 0;
                int length = 0;
                int isVertical = 0;
                
                // Déterminer orientation et position
                int vCount = 0;
                for (int k = 0; k < GRID; k++) {
                    if (g[k][j] == shipId || g[k][j] == -shipId || g[k][j] <= -200) {
                        vCount++;
                        if (k < i) position++;
                    }
                }
                
                int hCount = 0;
                int hPos = 0;
                for (int k = 0; k < GRID; k++) {
                    if (g[i][k] == shipId || g[i][k] == -shipId || g[i][k] <= -200) {
                        hCount++;
                        if (k < j) hPos++;
                    }
                }
                
                if (vCount > 1) {
                    isVertical = 1;
                    length = vCount;
                } else {
                    isVertical = 0;
                    length = hCount;
                    position = hPos;
                }
                
                // Utiliser le design ultra-réaliste
                drawRealisticShip(gfx, x, y, CELL, isVertical, position, length, shipId);
            }
            else {
                // Eau normale avec vagues animées
                drawWaves(gfx->renderer, x, y, CELL - 1, CELL - 1, currentTime);
            }
            
            // Grille de séparation subtile
            SDL_SetRenderDrawBlendMode(gfx->renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(gfx->renderer, 255, 255, 255, 30);
            SDL_RenderLine(gfx->renderer, x + CELL - 1, y, x + CELL - 1, y + CELL);
            SDL_RenderLine(gfx->renderer, x, y + CELL - 1, x + CELL, y + CELL - 1);
            SDL_SetRenderDrawBlendMode(gfx->renderer, SDL_BLENDMODE_NONE);
        }
    }
}






void drawTurnScreen(GraphicsContext *gfx, int player)
{
    // ===== Fond =====
    SDL_SetRenderDrawColor(gfx->renderer, 15, 25, 40, 255);
    SDL_RenderClear(gfx->renderer);

    // ===== Boîte centrale =====
    int boxW = WIDTH / 2;
    int boxH = HEIGHT / 4;
    int boxX = (WIDTH - boxW) / 2;
    int boxY = (HEIGHT - boxH) / 2;

    SDL_FRect box = { boxX, boxY, boxW, boxH };

    // Ombre
    SDL_SetRenderDrawColor(gfx->renderer, 0, 0, 0, 120);
    SDL_FRect shadow = { boxX + 6, boxY + 6, boxW, boxH };
    SDL_RenderFillRect(gfx->renderer, &shadow);

    // Fond boîte
    SDL_SetRenderDrawColor(gfx->renderer, 40, 60, 90, 255);
    SDL_RenderFillRect(gfx->renderer, &box);

    // Bordure
    SDL_SetRenderDrawColor(gfx->renderer, 180, 200, 220, 255);
    SDL_RenderRect(gfx->renderer, &box);

    // ===== Indicateur joueur =====
    SDL_FRect playerBox = {
        boxX + boxW / 4,
        boxY + boxH / 2 - 25,
        boxW / 2,
        50
    };

    if (player == 0) {
        // Joueur 1 : Vert
        SDL_SetRenderDrawColor(gfx->renderer, 60, 180, 120, 255);
    } else {
        // Joueur 2 : Rouge
        SDL_SetRenderDrawColor(gfx->renderer, 200, 70, 70, 255);
    }

    SDL_RenderFillRect(gfx->renderer, &playerBox);

    // Bordure joueur
    SDL_SetRenderDrawColor(gfx->renderer, 255, 255, 255, 255);
    SDL_RenderRect(gfx->renderer, &playerBox);

    // ===== Texte simple "JOUEUR X" (symbolique) =====
    SDL_SetRenderDrawColor(gfx->renderer, 255, 255, 255, 255);

    int cx = playerBox.x + playerBox.w / 2;
    int cy = playerBox.y + 12;

    // Lettre P
    SDL_RenderLine(gfx->renderer, cx - 30, cy, cx - 30, cy + 20); // barre verticale
    SDL_RenderLine(gfx->renderer, cx - 30, cy, cx - 10, cy);     // haut
    SDL_RenderLine(gfx->renderer, cx - 10, cy, cx - 10, cy + 10);// côté droit
    SDL_RenderLine(gfx->renderer, cx - 30, cy + 10, cx - 10, cy + 10); // milieu

    // Numéro joueur
    if (player == 0) {
        // 1
        SDL_RenderLine(gfx->renderer, cx + 10, cy, cx + 10, cy + 20);
    } else {
        // 2
        SDL_RenderLine(gfx->renderer, cx + 5, cy, cx + 25, cy);
        SDL_RenderLine(gfx->renderer, cx + 25, cy, cx + 25, cy + 10);
        SDL_RenderLine(gfx->renderer, cx + 5, cy + 10, cx + 25, cy + 10);
        SDL_RenderLine(gfx->renderer, cx + 5, cy + 10, cx + 5, cy + 20);
        SDL_RenderLine(gfx->renderer, cx + 5, cy + 20, cx + 25, cy + 20);
    }

    SDL_RenderPresent(gfx->renderer);
}

void drawShipPreview(GraphicsContext *gfx, int shipSize, int vertical) {
    SDL_FRect bg = { GRID * CELL, 0, GRID * CELL, HEIGHT };
    SDL_SetRenderDrawColor(gfx->renderer, 10, 10, 10, 255);
    SDL_RenderFillRect(gfx->renderer, &bg);

    int centerX = GRID * CELL + (GRID * CELL) / 2;
    int centerY = HEIGHT / 2;

    int width, height;
    if (shipSize > 0) {
        width  = vertical ? 1 : shipSize;
        height = vertical ? shipSize : 1;
    } else {
        width  = vertical ? 2 : 5;
        height = vertical ? 5 : 2;
    }

    int startX = centerX - (width * CELL) / 2;
    int startY = centerY - (height * CELL) / 2;

    SDL_FRect cell;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            cell.x = startX + j * CELL;
            cell.y = startY + i * CELL;
            cell.w = CELL - 2;
            cell.h = CELL - 2;

            SDL_SetRenderDrawColor(gfx->renderer, 180, 180, 180, 255);
            SDL_RenderFillRect(gfx->renderer, &cell);

            SDL_SetRenderDrawColor(gfx->renderer, 0, 0, 0, 255);
            SDL_RenderRect(gfx->renderer, &cell);
        }
    }
}

void drawShipGhost(GraphicsContext *gfx, int grid[GRID][GRID], int mouseX, int mouseY,
                   int shipSize, int vertical) {
    int gx = mouseX / CELL;
    int gy = mouseY / CELL;

    if (gx < 0 || gx >= GRID || gy < 0 || gy >= GRID) return;

    int width, height;
    if (shipSize > 0) {
        width  = vertical ? 1 : shipSize;
        height = vertical ? shipSize : 1;
    } else {
        width  = vertical ? 2 : 5;
        height = vertical ? 5 : 2;
    }

    int valid = 1;
    if (gx + width > GRID || gy + height > GRID) {
        valid = 0;
    } else {
        for (int i = gy - 1; i <= gy + height; i++) {
            for (int j = gx - 1; j <= gx + width; j++) {
                if (i < 0 || i >= GRID || j < 0 || j >= GRID)
                    continue;
                if (grid[i][j] != 0)
                    valid = 0;
            }
        }
    }

    if (valid)
        SDL_SetRenderDrawColor(gfx->renderer, 0, 200, 0, 120);
    else
        SDL_SetRenderDrawColor(gfx->renderer, 200, 0, 0, 120);

    SDL_SetRenderDrawBlendMode(gfx->renderer, SDL_BLENDMODE_BLEND);

    SDL_FRect r;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            r.x = (gx + j) * CELL + 1;
            r.y = (gy + i) * CELL + 1;
            r.w = CELL - 2;
            r.h = CELL - 2;
            SDL_RenderFillRect(gfx->renderer, &r);
        }
    }

    SDL_SetRenderDrawBlendMode(gfx->renderer, SDL_BLENDMODE_NONE);
}

void placeShipsSDL(GraphicsContext *gfx, Player *p, ShipConfig *config, int playerNum) {
    int currentShip = 0;
    int vertical = 0;
    SDL_Event e;
    int placing = 1;

    while(placing) {
        SDL_SetRenderDrawColor(gfx->renderer, 20, 20, 20, 255);
        SDL_RenderClear(gfx->renderer);

        drawGrid(gfx, p->grid, 0, 1);

        float mouseXF, mouseYF;
        SDL_GetMouseState(&mouseXF, &mouseYF);
        int mouseX = (int)mouseXF;
        int mouseY = (int)mouseYF;

        drawShipGhost(gfx, p->grid, mouseX, mouseY, config->sizes[currentShip], vertical);
        drawShipPreview(gfx, config->sizes[currentShip], vertical);

        SDL_RenderPresent(gfx->renderer);

        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_EVENT_QUIT) exit(0);
            else if(e.type == SDL_EVENT_KEY_DOWN) {
                if(e.key.scancode == SDL_SCANCODE_V) {
                    vertical = !vertical;
                }
            }
            else if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                int x = e.button.y / CELL;
                int y = e.button.x / CELL;

                if (config->sizes[currentShip] < 0) {
                    if (canPlaceShip5x2Manual(p->grid, x, y, vertical)) {
                        placeShip5x2Manual(p->grid, x, y, currentShip + 1, vertical);
                        currentShip++;
                    } else {
                        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Erreur",
                            "Placement impossible pour le bateau 5x2", gfx->window);
                    }
                } else {
                    if (canPlaceShip(p->grid, x, y, config->sizes[currentShip], vertical)) {
                        for (int i = 0; i < config->sizes[currentShip]; i++) {
                            int nx = x + (vertical ? i : 0);
                            int ny = y + (vertical ? 0 : i);
                            p->grid[nx][ny] = currentShip + 1;
                        }
                        currentShip++;
                    } else {
                        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Erreur",
                            "Placement impossible", gfx->window);
                    }
                }

                if (currentShip >= SHIP_COUNT) {
                    placing = 0;
                    break;
                }
            }
        }
    }
}
