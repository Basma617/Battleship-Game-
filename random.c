#include <SDL3/SDL.h>
#include "types.h"

#include "random.h"


int myRand() {
    seed = seed * 1103515245 + 12345;
    return (seed >> 16) & 0x7FFF;
}