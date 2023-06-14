#include <stdio.h>
#include "opCodeHandlers.c"
#include "SDL.h"
#include <unistd.h>


static void (*opCodeTable[16])();
// Initializes opcode table so the appropriate index can be called by main
void initalizeOpCodeTable() {
    opCodeTable[0x0] = handle0x0;
    opCodeTable[0x1] = handle0x1;
    opCodeTable[0x2] = handle0x2;
    opCodeTable[0x3] = handle0x3;
    opCodeTable[0x4] = handle0x4;
    opCodeTable[0x5] = handle0x5;
    opCodeTable[0x6] = handle0x6;
    opCodeTable[0x7] = handle0x7;
    opCodeTable[0x8] = handle0x8;
    opCodeTable[0x9] = handle0x9;
    opCodeTable[0xa] = handle0xa;
    opCodeTable[0xb] = handle0xb;
    opCodeTable[0xc] = handle0xc;
    opCodeTable[0xd] = handle0xd;
    opCodeTable[0xe] = handle0xe;
    opCodeTable[0xf] = handle0xf;
}

// initializes chip struct
Chip* InitializeChip(const char romName[]) {
    Chip* chip = (Chip*) malloc(sizeof(Chip));

    // sprite font for chip-8
    u_int8_t font[] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    // program counter starts at 0x200 (512 in decimal)
    chip->pc = 0x200;   
    // copy font anywhere between 0x00 and 0x200
    memcpy(&chip->ram[0], font, sizeof(font)); 
    FILE *rom = fopen(romName, "rb"); 
    if (rom == NULL) {
        printf("INVALID FILE PATH");
    }
    // get size of file
    fseek(rom, 0, SEEK_END);
    const size_t romSize = ftell(rom);
    rewind(rom);
    // read ROM data into the chip's RAM
    fread(&chip->ram[0x200], romSize, 1, rom);


    chip->emuState = RUNNING;
    // Initialize stack pointer
    chip->sp = &chip->stack[0];
    
    fclose(rom);

    return chip; 
}

// handles keyboard events
void eventHandler(Chip* chip) {
    SDL_Event windowEvent;
    while (SDL_PollEvent(&windowEvent)) {
        switch(windowEvent.type) {
            case SDL_QUIT:
                chip->emuState = IDLE;
                return;
            case SDL_KEYDOWN:
                switch(windowEvent.key.keysym.sym) {
                    case SDLK_SPACE:
                        if (chip->emuState == RUNNING) {
                            chip->emuState = PAUSED;
                        } else {
                            chip->emuState = RUNNING;
                        }
                        break;
                    // sets emulator state to IDLE(will stop emulation)
                    case SDLK_ESCAPE:
                        chip->emuState = IDLE;
                        return;
                    // If key is hit, update chip object with key
                    case SDLK_0: chip->keys[0] = true; break;
                    case SDLK_1: chip->keys[1] = true; break;
                    case SDLK_2: chip->keys[2] = true; break;
                    case SDLK_3: chip->keys[3] = true; break;
                    case SDLK_4: chip->keys[4] = true; break;
                    case SDLK_5: chip->keys[5] = true; break;
                    case SDLK_6: chip->keys[6] = true; break;
                    case SDLK_7: chip->keys[7] = true; break;
                    case SDLK_8: chip->keys[8] = true; break;
                    case SDLK_9: chip->keys[9] = true; break;
                    case SDLK_a: chip->keys[10] = true; break;
                    case SDLK_b: chip->keys[11] = true; break;
                    case SDLK_c: chip->keys[12] = true; break;
                    case SDLK_d: chip->keys[13] = true; break;
                    case SDLK_e: chip->keys[14] = true; break;
                    case SDLK_f: chip->keys[15] = true; break;
                    default:
                        break;
                }
                break;
            // when key is released, update chip object
            case SDL_KEYUP:
                 switch(windowEvent.key.keysym.sym) {
                    case SDLK_0: chip->keys[0] = false; break;
                    case SDLK_1: chip->keys[1] = false; break;
                    case SDLK_2: chip->keys[2] = false; break;
                    case SDLK_3: chip->keys[3] = false; break;
                    case SDLK_4: chip->keys[4] = false; break;
                    case SDLK_5: chip->keys[5] = false; break;
                    case SDLK_6: chip->keys[6] = false; break;
                    case SDLK_7: chip->keys[7] = false; break;
                    case SDLK_8: chip->keys[8] = false; break;
                    case SDLK_9: chip->keys[9] = false; break;
                    case SDLK_a: chip->keys[10] = false; break;
                    case SDLK_b: chip->keys[11] = false; break;
                    case SDLK_c: chip->keys[12] = false; break;
                    case SDLK_d: chip->keys[13] = false; break;
                    case SDLK_e: chip->keys[14] = false; break;
                    case SDLK_f: chip->keys[15] = false; break;
                 }
                break;
            }
        }
}
int width = 64;
int height = 32;

int main(int argc, char** argv) {
    (void) argc;
    const char* romName = argv[1];

    // Initializations
    Chip* chip = InitializeChip(romName);
    sdlFeatures* sdl = malloc(sizeof(sdlFeatures));
    initalizeOpCodeTable();
    SDL_Init( SDL_INIT_EVERYTHING );
    sdl->window = SDL_CreateWindow("Hello Emulation", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width * 10, height * 10, SDL_WINDOW_ALLOW_HIGHDPI);
    sdl->renderer = SDL_CreateRenderer(sdl->window, -1, SDL_RENDERER_ACCELERATED);

    if (sdl->window == NULL) {
        printf("COULD NOT CREATE WINDOW");
        return 0;
    }
    SDL_SetRenderDrawColor(sdl->renderer, 0, 0, 0, 1);
    SDL_RenderClear(sdl->renderer);

    while (chip->emuState != IDLE) {
        // handles keyboard presses
        eventHandler(chip);

        // do not run any instructions while emulator is paused
        if (chip->emuState == PAUSED) {
            continue;
        }
        u_int64_t startTime = SDL_GetPerformanceCounter();
        // This loop allows multiple instructions to be run before the frame is updated
        for (int i = 0; i < 700 / 60; i++) {
            // print address, opcode, and description of operation(for debugging purposes)
            printf("ADDRESS: 0x%04X, Opcode: 0x%04X   ", chip->pc, (chip->ram[chip->pc] << 8) | chip->ram[chip->pc + 1]);
            u_int8_t* op = chip->ram + chip->pc;
            // perform instruction
            opCodeTable[op[0] >> 4](chip, op);
        }
        
        // based on time taken for the batch of instructions to run, delay the updating of the frame
        u_int64_t endTime = SDL_GetPerformanceCounter();
        const double timeTaken = (double) ((endTime - startTime) * 1000) / SDL_GetPerformanceCounter();
        if (16.67f > timeTaken) {
            SDL_Delay(16.67f - timeTaken);
        } 
        
        // based on the current state of the screen object in the chip struct, update the sdl window
        SDL_Rect rectangle = {.x = 0, .y = 0, .w = 20, .h = 20};
        for (int i = 0; i < 2048; i++) {
            rectangle.x = (i % width) * 20;
            rectangle.y = (i / width) * 20;
            if (chip->screen[i]) {
                SDL_SetRenderDrawColor(sdl->renderer, 255, 255, 255, 1);
            } else {
                SDL_SetRenderDrawColor(sdl->renderer, 0, 0, 0, 1);
            }
            SDL_RenderFillRect(sdl->renderer, &rectangle);

            SDL_SetRenderDrawColor(sdl->renderer, 0, 0, 0, 1);
            SDL_RenderDrawRect(sdl->renderer, &rectangle);

            
        }
        // render to sdl window
        SDL_RenderPresent(sdl->renderer);

        // decrement timers
        chip->delayTimer = chip->delayTimer > 0 ? chip->delayTimer - 1 : 0;
        chip->soundTimer = chip->soundTimer > 0 ? chip->soundTimer - 1 : 0;
    }
        
            
        
    // once loop has exited, emulation is finished, destroy sdl features, free dynamic memory, and exit program
    SDL_DestroyRenderer(sdl->renderer);
    SDL_DestroyWindow(sdl->window);
    SDL_Quit();
    free(chip);
    free(sdl);
    // return EXIT_SUCCESS;
    return 1;
}

