#ifndef INSTRUCT_H_
#define INSTRUCT_H_

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


void loadROM (chip8* chip, char* filename);
void initalizeChip (chip8* chip);
int8_t randByte ();

void Table0 (chip8* chip);
void Table8 (chip8* chip);
void TableE (chip8* chip);
void TableF (chip8* chip);
void FunctionTable(chip8* chip);

void Cycle(chip8* chip);

void OP_00E0(chip8* chip);
void OP_00EE(chip8* chip);
void OP_1nnn(chip8* chip);
void OP_2nnn(chip8* chip);
void OP_3xkk(chip8* chip);
void OP_4xkk(chip8* chip);
void OP_5xy0(chip8* chip);
void OP_6xkk(chip8* chip);
void OP_7xkk(chip8* chip);
void OP_8xy0(chip8* chip);
void OP_8xy1(chip8* chip);
void OP_8xy2(chip8* chip);
void OP_8xy3(chip8* chip);
void OP_8xy4(chip8* chip);
void OP_8xy5(chip8* chip);
void OP_8xy6(chip8* chip);
void OP_8xy7(chip8* chip);
void OP_8xyE(chip8* chip);
void OP_9xy0(chip8* chip);
void OP_Annn(chip8* chip);
void OP_Bnnn(chip8* chip);
void OP_Cxkk(chip8* chip);
void OP_Dxyn(chip8* chip);
void OP_Ex9E(chip8* chip);
void OP_ExA1(chip8* chip);
void OP_Fx07(chip8* chip);
void OP_Fx0A(chip8* chip);
void OP_Fx15(chip8* chip);
void OP_Fx18(chip8* chip);
void OP_Fx1E(chip8* chip);
void OP_Fx29(chip8* chip);
void OP_Fx33(chip8* chip);
void OP_Fx55(chip8* chip);
void OP_Fx65(chip8* chip);
#endif