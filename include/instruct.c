#include "instruct.h"
#include <stdlib.h>
#include <stdio.h>

#define START_ADDRESS 0x200
#define FONTSET_SIZE 16 * 5 // 16 characters and 5 bytes
#define FONTSET_START_ADDRESS 0x50
#define VIDEO_WIDTH 64
#define VIDEO_HEIGHT 32

/*
For an opcode
nnn refers to an address (last 3 nibbles)
nn refers to an 8 bit number (last 2 nibbles)
n referst to a 4 bit number (last nibble)
X looks up one of the 16 registers (second nibble)
Y looks up one of the 16 registers (third nibble)
*/

// index just stores memory addresses to be used in operations that are too big for 8 bit registers

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

    for (unsigned int i = 0; i < FONTSET_SIZE; i++){
        chip->memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }

}

// pitch is the number of bytes in a row of pixel data
void Update (SDL_Texture * texture, SDL_Renderer * render, void const * buffer, int pitch){
	SDL_UpdateTexture(texture, NULL, buffer, pitch);
	SDL_RenderClear(render);
	SDL_RenderCopy(render, texture, NULL, NULL);
	SDL_RenderPresent(render);
}

int Input (chip8* chip){
    int output = 0;
    SDL_Event event;

    while (SDL_PollEvent(&event)){
        switch (event.type){
			case SDL_QUIT:{
					output = -1;
				} break;
				case SDL_KEYDOWN:{
					switch (event.key.keysym.sym){
						case SDLK_ESCAPE:{
							output = -1;
						} break;

						case SDLK_x:{
							chip->keypad[0] = 1;
						} break;

						case SDLK_1:{
							chip->keypad[1] = 1;
						} break;

						case SDLK_2:{
							chip->keypad[2] = 1;
						} break;

						case SDLK_3:{
							chip->keypad[3] = 1;
						} break;

						case SDLK_q:{
							chip->keypad[4] = 1;
						} break;

						case SDLK_w:{
							chip->keypad[5] = 1;
						} break;

						case SDLK_e:{
							chip->keypad[6] = 1;
						} break;

						case SDLK_a:{
							chip->keypad[7] = 1;
						} break;

						case SDLK_s:{
							chip->keypad[8] = 1;
						} break;

						case SDLK_d:{
							chip->keypad[9] = 1;
						} break;

						case SDLK_z:{
							chip->keypad[0xA] = 1;
						} break;

						case SDLK_c:{
							chip->keypad[0xB] = 1;
						} break;

						case SDLK_4:{
							chip->keypad[0xC] = 1;
						} break;

						case SDLK_r:{
							chip->keypad[0xD] = 1;
						} break;

						case SDLK_f:{
							chip->keypad[0xE] = 1;
						} break;

						case SDLK_v:{
							chip->keypad[0xF] = 1;
						} break;
					}
				} break;

				case SDL_KEYUP:
				{
					switch (event.key.keysym.sym)
					{
						case SDLK_x:{
							chip->keypad[0] = 0;
						} break;

						case SDLK_1:{
							chip->keypad[1] = 0;
						} break;

						case SDLK_2:{
							chip->keypad[2] = 0;
						} break;

						case SDLK_3:{
							chip->keypad[3] = 0;
						} break;

						case SDLK_q:{
							chip->keypad[4] = 0;
						} break;

						case SDLK_w:{
							chip->keypad[5] = 0;
						} break;

						case SDLK_e:{
							chip->keypad[6] = 0;
						} break;

						case SDLK_a:{
							chip->keypad[7] = 0;
						} break;

						case SDLK_s:{
							chip->keypad[8] = 0;
						} break;

						case SDLK_d:{
							chip->keypad[9] = 0;
						} break;

						case SDLK_z:{
							chip->keypad[0xA] = 0;
						} break;

						case SDLK_c:{
							chip->keypad[0xB] = 0;
						} break;

						case SDLK_4:{
							chip->keypad[0xC] = 0;
						} break;

						case SDLK_r:{
							chip->keypad[0xD] = 0;
						} break;

						case SDLK_f:{
							chip->keypad[0xE] = 0;
						} break;

						case SDLK_v:{
							chip->keypad[0xF] = 0;
						} break;
					}
				} break;
			}
		}

		return output;
}

void loadROM(chip8* chip, char* filename){
    FILE* f = fopen(filename, "rb+");
    if (!f) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    fseek(f, 0, SEEK_END);
    long size = ftell(f); // number of bytes from the beginning of the file
    fseek(f, 0, SEEK_SET); 

    char* buffer[size];

    fgets(buffer, size+1, f);
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
    uint8_t height = chip->opcode & 0x000Fu;

    uint8_t xPos = chip->registers[Vx] % VIDEO_WIDTH;
    uint8_t yPos = chip->registers[Vy] % VIDEO_HEIGHT;

    chip->registers[0xF] = 0;

    for (unsigned int row = 0; row < height; row++){
        // move to next byte when iterating memory address
        uint8_t spriteByte = chip->memory[chip->index + row];
        for (unsigned int col = 0; col < 8; col++){
            uint8_t spritePixel = spriteByte & (0x80u >> col);
            // Use starting point and current position when drawing sprite to calculate array index
            // for video output
            uint32_t *screenPixel = &(chip->video[(yPos + row) * VIDEO_WIDTH + (xPos + col)]);

            if (spritePixel){
                if (*screenPixel == 0xFFFFFFFF){
                    chip->registers[0xF] = 1;
                }
                *screenPixel ^= 0xFFFFFFFF;
            }
        }
    }
}

// Skip next instruction if key with the value of Vx is pressed
void OP_Ex9E(chip8* chip){
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;

    uint8_t key = chip->registers[Vx];

    if (chip->keypad[key]){
        chip->program_counter += 2;
    }
}

// Skip next instruction if key with the value of Vx is not pressed
void OP_ExA1(chip8* chip){
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;

    uint8_t key = chip->registers[Vx];

    if (!chip->keypad[key]){
        chip->program_counter += 2;
    }
}

// Set Vx = delay timer value
void OP_Fx07(chip8* chip){
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
    chip->registers[Vx] = chip->delayTimer;
}

// Wait for a key press, store the value of the key in Vx
void OP_Fx0A(chip8* chip){
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	if (chip->keypad[0])
	{
		chip->registers[Vx] = 0;
	}
	else if (chip->keypad[1])
	{
		chip->registers[Vx] = 1;
	}
	else if (chip->keypad[2])
	{
		chip->registers[Vx] = 2;
	}
	else if (chip->keypad[3])
	{
		chip->registers[Vx] = 3;
	}
	else if (chip->keypad[4])
	{
		chip->registers[Vx] = 4;
	}
	else if (chip->keypad[5])
	{
		chip->registers[Vx] = 5;
	}
	else if (chip->keypad[6])
	{
		chip->registers[Vx] = 6;
	}
	else if (chip->keypad[7])
	{
		chip->registers[Vx] = 7;
	}
	else if (chip->keypad[8])
	{
		chip->registers[Vx] = 8;
	}
	else if (chip->keypad[9])
	{
		chip->registers[Vx] = 9;
	}
	else if (chip->keypad[10])
	{
		chip->registers[Vx] = 10;
	}
	else if (chip->keypad[11])
	{
		chip->registers[Vx] = 11;
	}
	else if (chip->keypad[12])
	{
		chip->registers[Vx] = 12;
	}
	else if (chip->keypad[13])
	{
		chip->registers[Vx] = 13;
	}
	else if (chip->keypad[14])
	{
		chip->registers[Vx] = 14;
	}
	else if (chip->keypad[15])
	{
		chip->registers[Vx] = 15;
	}
	else
	{
		chip->program_counter -= 2;
	}
}

// Set delay timer = Vx
void OP_Fx15(chip8* chip){
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
    chip->delayTimer = chip->registers[Vx];
}

// Set sound timer = Vx
void OP_Fx18(chip8* chip){
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
    chip->soundTimer = chip->registers[Vx]; 
}

// Set I = I + Vx
void OP_Fx1E(chip8* chip){
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
    chip->index += chip->registers[Vx];
}

// Set I = location of sprite for digit Vx
void OP_Fx29(chip8* chip){
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
    uint8_t digit = chip->registers[Vx];
    // begin address for character is equal to itself * 5 bytes
    chip->index = FONTSET_START_ADDRESS + (5 * digit);
}

// Takes value and stores the the 100ths place value at memory location i
// the 10s place value at i+1 and ones at i+2
void OP_Fx33(chip8* chip){
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
    uint8_t value = chip->registers[Vx];

    chip->memory[chip->index + 2] = value % 10;
    value /= 10;

    chip->memory[chip->index + 1] = value % 10;
    value /= 10;

    chip->memory[chip->index] = value % 10;
}

// Store registers V0 through Vx in memory starting at location I
void OP_Fx55(chip8* chip){
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;

    for (uint8_t i = 0; i <= Vx; i++){
        chip->memory[chip->index + i] = chip->registers[i];
    }
}

// Read registers V0 through Vx from memory starting at location I
void OP_Fx65(chip8* chip){
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;

    for (uint8_t i = 0; i <= Vx; i++){
       chip->registers[i] =  chip->memory[chip->index + i];
    }
}
void Table0 (chip8* chip){
    void (*table0[0xE + 1])(chip8* chip);
    table0[0x0] = OP_00E0;
    table0[0xE] = OP_00EE;
    (*table0[(chip->opcode & 0x000Fu)])(chip);
}
void Table8 (chip8* chip){
    void (*table8[0xE + 1])(chip8* chip);
    table8[0x0] = OP_8xy0;
    table8[0x1] = OP_8xy1;
    table8[0x2] = OP_8xy2;
    table8[0x3] = OP_8xy3;
    table8[0x4] = OP_8xy4;
    table8[0x5] = OP_8xy5;
    table8[0x6] = OP_8xy6;
    table8[0x7] = OP_8xy7;
    table8[0xE] = OP_8xyE;
    (*table8[(chip->opcode & 0x000Fu)])(chip);
}
void TableE (chip8* chip){
    void (*tableE[0xE + 1])(chip8* chip);
    tableE[0x1] = OP_ExA1;
    tableE[0xE] = OP_Ex9E;
    (*tableE[(chip->opcode & 0x000Fu)])(chip);

}
void TableF (chip8* chip){
    void (*tableF[0x65 +1])(chip8* chip);
    tableF[0x07] = OP_Fx07;
    tableF[0x0A] = OP_Fx0A;
    tableF[0x15] = OP_Fx15;
    tableF[0x18] = OP_Fx18;
    tableF[0x1E] = OP_Fx1E;
    tableF[0x29] = OP_Fx29;
    tableF[0x33] = OP_Fx33;
    tableF[0x55] = OP_Fx55;
    tableF[0x65] = OP_Fx65;
    (*tableF[(chip->opcode & 0x00FFu)])(chip);
}

// take opcode and give function
void FunctionTable(chip8* chip){
    void (*table[0xF + 1])(chip8* chip);
    table[0x0] = Table0;
    table[0x1] = OP_1nnn;
    table[0x2] = OP_2nnn;
    table[0x3] = OP_3xkk;
    table[0x4] = OP_4xkk;
    table[0x5] = OP_5xy0;
    table[0x6] = OP_6xkk;
    table[0x7] = OP_7xkk;
    table[0x8] = Table8;
    table[0x9] = OP_9xy0;
    table[0xA] = OP_Annn;
    table[0xB] = OP_Bnnn;
    table[0xC] = OP_Cxkk;
    table[0xD] = OP_Dxyn;
    table[0xE] = TableE;
    table[0xF] = TableF;
    (*table[(chip->opcode & 0xF000u) >> 12u])(chip);
}

void Cycle(chip8* chip){
    chip->opcode = (chip->memory[chip->program_counter] << 8u) | chip->memory[chip->program_counter + 1];
    chip->program_counter += 2;
    FunctionTable(chip);

    if (chip->delayTimer > 0){
        chip->delayTimer--;
    }
    if (chip->soundTimer > 0){
        chip->soundTimer--;
    }
}