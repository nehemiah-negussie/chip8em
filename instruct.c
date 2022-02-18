#include "instruct.h"
#include <stdlib.h>
#include <stdio.h>

#define START_ADDRESS 0x200
#define FONTSET_SIZE 16 * 5 // 16 characters and 5 bytes
#define FONTSET_START_ADDRESS 0x50

/*
For an opcode
nnn refers to an address (last 3 nibbles)
nn refers to an 8 bit number (last 2 nibbles)
n referst to a 4 bit number (last nibble)
X looks up one of the 16 registers (second nibble)
Y looks up one of the 16 registers (third nibble)
*/

typedef struct chip8 {
    // uint8_t represents 1 byte
    uint8_t registers[16];
    uint8_t memory[4096];
    uint16_t index;
	uint16_t program_counter;
	uint16_t stack[16];
	uint8_t stack_pointer;
	uint8_t delayTimer;
	uint8_t soundTimer;
	uint8_t keypad[16];
	uint32_t video[64 * 32];
	uint16_t opcode;

} chip8;

void initalizeChip (chip8* chip){
    chip->program_counter = START_ADDRESS;

    uint8_t fontset[FONTSET_SIZE] =
    {
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

    for (unsigned int i = 0; i = 0; i < FONTSET_SIZE){
        chip->memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }
}

void loadROM(chip8* chip, char* filename){
    FILE* f = fopen(filename, "rb");
    if (!f) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    fseek(f, 0, SEEK_SET); 

    char* buffer[size];

    if (fgets(buffer, size, f)!= NULL){
        puts(buffer);
    }
    fclose(f);
    for (long i = 0; i <  size; i++){
        chip->memory[START_ADDRESS + i] = buffer[i];
    }
}
    
uint8_t randByte (){
    return rand() % 256;
}

// Clear display
void OP_00E0(chip8* chip){
    // set whole video array to 0
    memset(chip->video, 0, sizeof chip->video);
}

// Return from subroutine
void OP_00EE(chip8* chip){
    chip->stack_pointer--;
    chip->program_counter = chip->stack[chip->stack_pointer];
}

// Jump to address nnn
void OP_1nnn(chip8* chip){
    uint16_t address = chip->opcode & 0x0FFFu;
    chip->program_counter = address;
}

// Call subroutine at nnn
void OP_2nnn(chip8* chip){
    uint16_t address = chip->opcode & 0x0FFFu;

    chip->stack[chip->stack_pointer] = chip->program_counter;
    chip->stack_pointer++;
    chip->program_counter = address;
}

// Skip next instruction if Vx = kk
void OP_3xkk(chip8* chip){
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t byte = chip->opcode & 0x00FFu;

    if (chip->registers[Vx] == byte){
        chip->program_counter += 2;
    }
}

// Skip next instruction if Vx != kk
void OP_4xkk(chip8* chip){
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t byte = chip->opcode & 0x00FFu;

    if (chip->registers[Vx] != byte){
        chip->program_counter += 2;
    }
}

// Skip next instruction if Vx = Vy
void OP_5xy0(chip8* chip){
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (chip->opcode & 0x00F0u) >> 4u;

	if (chip->registers[Vx] == chip->registers[Vy])
	{
		chip->program_counter += 2;
	}
}

// Set Vx = kk
void OP_6xkk(chip8* chip){
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t byte = chip->opcode & 0x00FFu;

	chip->registers[Vx] = byte;
}

// Set Vx = Vx + kk
void OP_7xkk(chip8* chip){
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t byte = chip->opcode & 0x00FFu;

	chip->registers[Vx] += byte;
}

// Set Vx = Vy
void OP_8xy0(chip8* chip){
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (chip->opcode & 0x00F0u) >> 4u;

	chip->registers[Vx] = chip->registers[Vy];
}

// Set Vx = Vx OR Vy
void OP_8xy1(chip8* chip){
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (chip->opcode & 0x00F0u) >> 4u;

	chip->registers[Vx] |= chip->registers[Vy];
}

// Set Vx = Vx AND Vy
void OP_8xy2(chip8* chip){
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (chip->opcode & 0x00F0u) >> 4u;

	chip->registers[Vx] &= chip->registers[Vy];
}

// Set Vx = Vx XOR Vy
void OP_8xy3(chip8* chip){
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (chip->opcode & 0x00F0u) >> 4u;

	chip->registers[Vx] ^= chip->registers[Vy];
}

// Set Vx = Vx + Vy, if greater than 255 set VF on
void OP_8xy4(chip8* chip){
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (chip->opcode & 0x00F0u) >> 4u;

    uint16_t sum = chip->registers[Vx] + chip->registers[Vy];

    if (sum > 255U){
        chip->registers[0xF] = 1;
    }
    else {
        chip->registers[0xF] = 0;
    }

    chip->registers[Vx] = sum & 0x00FFu;
}

// Set Vx = Vx - Vy, if Vx > Vy set VF on
void OP_8xy5(chip8* chip){
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (chip->opcode & 0x00F0u) >> 4u;
    
    if (chip->registers[Vx] < chip->registers[Vy]){
        chip->registers[0xF] = 1;
    }
    else {
        chip->registers[0xF] = 0;
    }
    chip->registers[Vx] -= chip->registers[Vy];
}

// Set Vx = Vx shift right by 1
// If the least significant bit is 1 and it gets shifted then set VF on
void OP_8xy6(chip8* chip){
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
    
    chip->registers[0xF] = (chip->registers[Vx] & 0x1u);
    
    chip->registers[Vx] >>= 1;
}

// Set Vx = Vy - Vx
// If Vy > Vx, set VF on
void OP_8xy7(chip8* chip){
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (chip->opcode & 0x00F0u) >> 4u;
    
    if (chip->registers[Vy] < chip->registers[Vx]){
        chip->registers[0xF] = 1;
    }
    else {
        chip->registers[0xF] = 0;
    }
    chip->registers[Vx] = chip->registers[Vy] - chip->registers[Vx];
}

// Set Vx = Vx shift left 1
// if most significant bit is 1 the set VF on
void OP_8xyE(chip8* chip){
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
    
    chip->registers[0xF] = (chip->registers[Vx] & 0x80u) >> 7u;
    
    chip->registers[Vx] <<= 1;
}

// Skip next instruction if Vx != Vy
void OP_9xy0(chip8* chip){
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (chip->opcode & 0x00F0u) >> 4u;

    if (chip->registers[Vx] != chip->registers[Vy])
	{
		chip->program_counter += 2;
	}
}

// Set I = nnn
void OP_Annn(chip8* chip){
    uint16_t address = chip->opcode & 0x0FFFu;
    
    chip->index = address;
}

// Jump to location nnn + V0
void OP_Bnnn(chip8* chip){
    uint16_t address = chip->opcode & 0x0FFFu;

    chip->program_counter = chip->registers[0] + address;
}

// Set Vx = random byte AND kk
void OP_Cxkk(chip8* chip){
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t byte = chip->opcode & 0x00FFu;

    chip->registers[Vx] = randByte() & byte;
}

// Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
void OP_Dxyn(chip8* chip){
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (chip->opcode & 0x00F0u) >> 4u;
    
}

void OP_Ex9E(chip8* chip){

}

void OP_ExA1(chip8* chip){

}

void OP_Fx07(chip8* chip){

}

void OP_Fx0A(chip8* chip){

}

void OP_Fx15(chip8* chip){

}

void OP_Fx18(chip8* chip){

}

void OP_Fx1E(chip8* chip){

}

void OP_Fx29(chip8* chip){

}

void OP_Fx33(chip8* chip){

}

void OP_Fx55(chip8* chip){

}

void OP_Fx65(chip8* chip){

}
