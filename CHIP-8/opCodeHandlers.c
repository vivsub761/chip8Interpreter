#include "main.h"
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

// This file handles all Chip8 instructions
// for information on each instruction or Chip-8 in general, see
// https://en.wikipedia.org/wiki/CHIP-8
// http://devernay.free.fr/hacks/chip8/C8TECH10.HTM

void handle0x0(Chip* chip, u_int8_t* op) {
    if (op[1] == 0xE0) {
        memset(&chip->screen[0], false, sizeof chip->screen);
        // printf("CLEAR SCREEN\n");
        chip->pc += 2;
    } else if (op[1] == 0xEE) {
        // printf("RETURN FROM SUBROUTINE TO ADDRESS 0x%04X\n", *(chip->sp - 1));
        chip->pc = *--chip->sp + 2;
    } 
}

void handle0x1(Chip* chip, u_int8_t* op) {
    chip->pc = (((op[0] & 0x0f) << 8) | op[1]);
}

void handle0x2(Chip* chip, u_int8_t* op) {
    // printf("CALL SUBROUTINE AT 0x%04X\n", (((op[0] & 0x0f) << 8) | op[1]));
    *chip->sp++ = chip->pc;
    chip->pc = (((op[0] & 0x0f) << 8) | op[1]);

}

void handle0x3(Chip* chip, u_int8_t* op) {
    u_int8_t regNumber = op[0] & 0xf;
    u_int8_t targetVal = op[1];
    if (chip->V[regNumber] == targetVal) {
        chip->pc += 2;
    }
    chip->pc += 2;
}

void handle0x4(Chip* chip, u_int8_t* op) {
    u_int8_t regNumber = op[0] & 0xf;
    u_int8_t targetVal = op[1];
    if (chip->V[regNumber] != targetVal) {
        chip->pc += 2;
    }
    chip->pc += 2;
}

void handle0x5(Chip* chip, u_int8_t* op) {
    u_int8_t reg1 = op[0] & 0x0f;
    u_int8_t reg2 = op[1] >> 4;
    if (chip->V[reg1] == chip->V[reg2]) {
        chip->pc += 2;
    }
    chip->pc += 2;
}

void handle0x6(Chip* chip, u_int8_t* op) {
    u_int8_t registerNumber = op[0] & 0x0f;
    chip->V[registerNumber] = op[1];
    chip->pc += 2;
    // printf("SET REGISTER V%d to NN (0x%02X)\n", registerNumber, op[1]);
}

void handle0x7(Chip* chip, u_int8_t* op) {
    u_int8_t immediate = op[1];
    u_int8_t regNum = op[0] & 0x0f;
        // printf("Set register V%X (0x%02X) += NN (0x%02X). Result: 0x%02X\n", regNum, chip->V[regNum], immediate, chip->V[regNum] + immediate);
    chip->V[regNum] += immediate;
    chip->pc += 2;

}

void handle0x8(Chip* chip, u_int8_t* op) {
    u_int8_t reg1 = op[0] & 0x0f;
    u_int8_t reg2 = op[1] >> 4;

    u_int8_t lsByte = op[1] & 0x0f;
    switch(lsByte) {
        case 0x00: chip->V[reg1] = chip->V[reg2]; break;
        case 0x01: chip->V[reg1] = chip->V[reg1] | chip->V[reg2]; break;
        case 0x02: chip->V[reg1] = chip->V[reg1] & chip->V[reg2]; break;
        case 0x03: chip->V[reg1] = chip->V[reg1] ^ chip->V[reg2]; break;
        case 0x04: 
            chip->V[15] = ((u_int16_t) chip->V[reg1] + (u_int16_t) chip->V[reg2]) > 255;
            chip->V[reg1] = chip->V[reg1] + chip->V[reg2];
            break;
        case 0x05:
            chip->V[15] = chip->V[reg1] >= chip->V[reg2];
            chip->V[reg1] -= chip->V[reg2];
            break;
        case 0x06:
            chip->V[15] = chip->V[reg1] % 2;
            chip->V[reg1] = chip->V[reg1] >> 1;
            break;
        case 0x07:
            chip->V[15] = chip->V[reg1] < chip->V[reg2];
            chip->V[reg1] = chip->V[reg2] - chip->V[reg1];
            break;
        case 0x0E:
            chip->V[15] = chip->V[reg1] >> 7;
            chip->V[reg1] = chip->V[reg1] << 1;
            break;
        default:
            printf("INVALID INSTRUCTION FOR OPCODE 0x08");
            break; 
    }
    chip->pc += 2;    
}

void handle0x9(Chip* chip, u_int8_t* op) {
    u_int8_t reg1 = op[0] & 0x0f;
    u_int8_t reg2 = op[1] >> 4;
    if (chip->V[reg1] != chip->V[reg2]) {
        chip->pc += 2;
    }
    chip->pc += 2;
}

void handle0xa(Chip* chip, u_int8_t* op) {
    // printf("SET I TO NNN (0x%04X)\n", ((op[0] & 0x0f) << 8) | op[1]);
    chip->I = ((op[0] & 0x0f) << 8) | op[1];
    chip->pc += 2;
}

void handle0xb(Chip* chip, u_int8_t* op) {
    chip->pc = (((op[0] & 0x0f) << 8) | op[1]) + chip->V[0];
}

void handle0xc(Chip* chip, u_int8_t* op) {
    srand(time(NULL));
    u_int8_t reg1 = op[0] & 0x0f;
    u_int8_t immediate = op[1];
    u_int8_t randomByte = rand() % 256;
    chip->V[reg1] = randomByte & immediate;
    chip->pc += 2;
}

void handle0xd(Chip* chip, u_int8_t* op) {
    u_int8_t n = op[1] & 0x0f;
    u_int8_t x = chip->V[op[0] & 0x0f] % 64; 
    u_int8_t y = chip->V[op[1] >> 4] % 32;
    const u_int8_t originalX = x;
    // printf("DRAW N (%u) height sprite at coords V%X (0x%02X), V%X (0x%02X) from memory location I (0x%04X)\n", n, op[0] & 0x0f, x, op[1] >> 4, y, chip->I);

    
    chip->V[15] = 0;

    for (u_int8_t i = 0; i < n; i++) {
        if (y == 32) break;
        x = originalX;
        const u_int8_t sprite = chip->ram[chip->I + i];
        for (u_int8_t j = 0; j < 8; j++) {
            if (x == 64) {
                break;
            }
            const bool spriteBit = (sprite & (1 << (7 - j)));
            bool *screenBit = &chip->screen[y * 64 + x]; 
            if (*screenBit && spriteBit) {
                chip->V[15] = 1;
            } 
            *screenBit ^= spriteBit;
            x += 1;
        }
        y += 1;
    }

    chip->pc += 2;
    

     
}

void handle0xe(Chip* chip, u_int8_t* op) {
    u_int8_t reg = op[0] & 0x0f;
    u_int8_t lsb = op[1];
    switch(lsb) {
        case 0x9E:
            if (chip->keys[chip->V[reg]] != 0) {
            chip->pc += 2;
            }
            break;
        case 0xA1:
            if (chip->keys[chip->V[reg]] == 0) {
                chip->pc += 2;
            }
            break;

    }
    chip->pc += 2;
    
}

void handle0xf(Chip* chip, u_int8_t* op) {
    u_int8_t regNum = op[0] & 0x0f;
    switch(op[1]) {
        case 0x07:
            chip->V[regNum] = chip->delayTimer;
            break;
        case 0x0A:
            for (u_int8_t i = 0; i < 16; i++) {
                if (chip->keys[i]) {
                    chip->V[regNum] = i;
                    chip->pc += 2;
                    return;
                }
            }
            chip->pc -= 2;
            
            break;
        case 0x15:
            chip->delayTimer = chip->V[regNum];
            break;
        case 0x18:
            chip->soundTimer = chip->V[regNum];
            break;
        case 0x1E:
            chip->I += chip->V[regNum]; 
            break;
        case 0x29:
            chip->I = chip->ram[chip->V[regNum]] * 5;
            break;
        case 0x33:
            chip->ram[chip->I] = chip->V[regNum] / 100;
            chip->ram[chip->I + 1] = (chip->V[regNum] / 10) % 10;
            chip->ram[chip->I + 2] = chip->V[regNum] % 10;
            break;
        case 0x55:
            for (u_int8_t i = 0; i < regNum + 1; i++) {
                chip->ram[chip->I + i] = chip->V[i];
            }
            break;
        case 0x65:
            for (u_int8_t i = 0; i < regNum + 1; i++) {
                chip->V[i] = chip->ram[chip->I + i];
            }
            break;
        default:
            printf("UNIMPLEMENTED INSTRUCTION\n");
            break;
    }
    chip->pc += 2;
}
