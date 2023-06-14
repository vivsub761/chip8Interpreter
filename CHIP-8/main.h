#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include "SDL.h"

#define BASE_FONT 0;

typedef enum {
    RUNNING,
    PAUSED,
    IDLE
} state;

typedef struct {
    u_int8_t V[16];
    u_int16_t I;
    u_int8_t soundTimer;
    u_int8_t delayTimer;
    u_int16_t pc;
    u_int16_t stack[12];
    u_int16_t *sp;
    u_int8_t ram[4096];
    bool screen[64*32];
    bool keys[16];
    int waitForKey;
    state emuState;
} Chip;

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
} sdlFeatures;

