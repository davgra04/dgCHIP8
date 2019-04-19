#include "instruction.h"

int return_four() {
    return 4;
}

// 00E0 - CLS
// Clear the display.
void instruction_clear_screen() {
    clear_display();
}

// 00EE - RET
// Return from a subroutine.
void instruction_return_subroutine() {
    program_counter = pop_stack();
}

// 1nnn - JP addr
// Jump to location nnn.
void instruction_jump(unsigned short instruction) {
    unsigned short address = instruction & 0x0FFF;
    program_counter = address;
}

// 2nnn - CALL addr
// Call subroutine at nnn.
void instruction_call_subroutine(unsigned short instruction) {
    unsigned short address = instruction & 0x0FFF;
    push_stack(program_counter);
    program_counter = address;
    // printf("DEBUG2: instruction: 0x%04x\n", instruction);
    // printf("DEBUG2: address: 0x%04x\n", address);
    // printf("DEBUG2: program_counter: 0x%04x\n", program_counter);
}

// 3xkk - SE Vx, byte
// Skip next instruction if Vx = kk.
void instruction_skip_equal_byte(unsigned short instruction) {
    unsigned char reg_idx = (instruction & 0x0F00) >> 8;
    unsigned char value = instruction & 0x00FF;
    if(reg[reg_idx] == value) {
        program_counter++;      // extra increment of program counter
    }
}

// 4xkk - SNE Vx, byte
// Skip next instruction if Vx != kk.
void instruction_skip_not_equal_byte(unsigned short instruction) {
    unsigned char reg_idx = (instruction & 0x0F00) >> 8;
    unsigned char value = instruction & 0x00FF;
    if(reg[reg_idx] != value) {
        program_counter++;      // extra increment of program counter
    }
}

// 5xy0 - SE Vx, Vy
// Skip next instruction if Vx = Vy.
void instruction_skip_equal_reg(unsigned short instruction) {
    unsigned char reg_x = (instruction & 0x0F00) >> 8;
    unsigned char reg_y = (instruction & 0x00F0) >> 4;
    if(reg[reg_x] == reg[reg_y]) {
        program_counter++;      // extra increment of program counter
    }
}

// 6xkk - LD Vx, byte
// Set Vx = kk.
void instruction_load_byte(unsigned short instruction) {
    unsigned char reg_idx = (instruction & 0x0F00) >> 8;
    unsigned char value = instruction & 0x00FF;
    reg[reg_idx] = value;
}

// 7xkk - ADD Vx, byte
// Set Vx = Vx + kk.
void instruction_add_byte(unsigned short instruction) {
    unsigned char reg_idx = (instruction & 0x0F00) >> 8;
    unsigned char value = instruction & 0x00FF;

    unsigned short sum = reg[reg_idx] + value;
    if(sum > 0xFF) {
        reg[0xF] = 0x1;
    }
    else {
        reg[0xF] = 0x0;
    }

    reg[reg_idx] = sum & 0xFF;
}

// 8xy0 - LD Vx, Vy
// Set Vx = Vy.
void instruction_load_reg(unsigned short instruction) {
    unsigned char reg_x = (instruction & 0x0F00) >> 8;
    unsigned char reg_y = (instruction & 0x00F0) >> 4;
    reg[reg_x] = reg[reg_y];
}

// 8xy1 - OR Vx, Vy
// Set Vx = Vx OR Vy.
void instruction_or(unsigned short instruction) {
    unsigned char reg_x = (instruction & 0x0F00) >> 8;
    unsigned char reg_y = (instruction & 0x00F0) >> 4;
    reg[reg_x] = reg[reg_x] | reg[reg_y];
}

// 8xy2 - AND Vx, Vy
// Set Vx = Vx AND Vy.
void instruction_and(unsigned short instruction) {
    unsigned char reg_x = (instruction & 0x0F00) >> 8;
    unsigned char reg_y = (instruction & 0x00F0) >> 4;
    reg[reg_x] = reg[reg_x] & reg[reg_y];
}

// 8xy3 - XOR Vx, Vy
// Set Vx = Vx XOR Vy.
void instruction_xor(unsigned short instruction) {
    unsigned char reg_x = (instruction & 0x0F00) >> 8;
    unsigned char reg_y = (instruction & 0x00F0) >> 4;
    reg[reg_x] = reg[reg_x] ^ reg[reg_y];
}

// 8xy4 - ADD Vx, Vy
// Set Vx = Vx + Vy, set VF = carry.
void instruction_add_reg(unsigned short instruction) {
    unsigned char reg_x = (instruction & 0x0F00) >> 8;
    unsigned char reg_y = (instruction & 0x00F0) >> 4;
    unsigned short sum = reg[reg_x] + reg[reg_y];
    if(sum > 0xFF) {
        reg[0xF] = 0x1;
    }
    else {
        reg[0xF] = 0x0;
    }

    reg[reg_x] = sum & 0xFF;
}

// 8xy5 - SUB Vx, Vy
// Set Vx = Vx - Vy, set VF = NOT borrow.
void instruction_sub_reg(unsigned short instruction) {
    unsigned char reg_x = (instruction & 0x0F00) >> 8;
    unsigned char reg_y = (instruction & 0x00F0) >> 4;
    if(reg[reg_x] > reg[reg_y]) {
        reg[reg_x] = reg[reg_x] - reg[reg_y];
        reg[0xF] = 0x1;
    }
    else {
        unsigned short sub = 0x100 + reg[reg_x] - reg[reg_y];
        reg[reg_x] = sub & 0xFF;
        reg[0xF] = 0x0;
    }
}

// 8xy6 - SHR Vx
// Set Vx = Vx SHR 1.
void instruction_shift_right(unsigned short instruction) {
    unsigned char reg_x = (instruction & 0x0F00) >> 8;
    if((reg[reg_x] & 0x1) == 0x1) {
        reg[0xF] = 0x1;
    }
    else {
        reg[0xF] = 0x0;
    }

    reg[reg_x] = reg[reg_x] >> 1;
}

// 8xy7 - SUBN Vx, Vy
// Set Vx = Vy - Vx, set VF = NOT borrow.
void instruction_subn_reg(unsigned short instruction) {
    unsigned char reg_x = (instruction & 0x0F00) >> 8;
    unsigned char reg_y = (instruction & 0x00F0) >> 4;
    if(reg[reg_y] > reg[reg_x]) {
        reg[reg_x] = reg[reg_y] - reg[reg_x];
        reg[0xF] = 0x1;
    }
    else {
        unsigned short sub = 0x100 + reg[reg_y] - reg[reg_x];
        reg[reg_x] = sub & 0xFF;
        reg[0xF] = 0x0;
    }
}

// 8xyE - SHL Vx {, Vy}
// Set Vx = Vx SHL 1.
void instruction_shift_left(unsigned short instruction) {
    unsigned char reg_x = (instruction & 0x0F00) >> 8;
    if((reg[reg_x] & 0x80) == 0x80) {
        reg[0xF] = 0x1;
    }
    else {
        reg[0xF] = 0x0;
    }

    reg[reg_x] = reg[reg_x] << 1;
}

// 9xy0 - SNE Vx, Vy
// Skip next instruction if Vx != Vy.
void instruction_skip_not_equal_reg(unsigned short instruction) {
    unsigned char reg_x = (instruction & 0x0F00) >> 8;
    unsigned char reg_y = (instruction & 0x00F0) >> 4;
    if(reg[reg_x] != reg[reg_y]) {
        program_counter++;      // extra increment of program counter
    }
}


// Annn - LD I, addr
// Set I = nnn.
void instruction_load_regI(unsigned short instruction) {
    unsigned short addr = instruction & 0x0FFF;
    reg_I = addr;
}

// Bnnn - JP V0, addr
// Jump to location nnn + V0.
void instruction_jump_reg(unsigned short instruction) {
    unsigned short address = instruction & 0x0FFF;
    program_counter = address + reg[0x0];
}


// Cxkk - RND Vx, byte
// Set Vx = random byte AND kk.
void instruction_rand(unsigned short instruction) {
    unsigned char reg_idx = (instruction & 0x0F00) >> 8;
    unsigned char value = instruction & 0x00FF;
    reg[reg_idx] = (rand() & 0xFF) & value;
}


// Dxyn - DRW Vx, Vy, nibble
// Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
void instruction_draw_sprite(unsigned short instruction) {
    // unsigned char reg_x = (instruction & 0x0F00) >> 8;
    // unsigned char reg_y = (instruction & 0x00F0) >> 4;
    // unsigned char bytes = instruction & 0x000F;
    // unsigned char byte_to_draw;

    // for(char i=0; i<bytes; i++) {
    //     byte_to_draw = read_program_byte(reg_I + i);
    // }
}


// Ex9E - SKP Vx
// Skip next instruction if key with the value of Vx is pressed.
// void berga(unsigned short instruction) {}


// ExA1 - SKNP Vx
// Skip next instruction if key with the value of Vx is not pressed.
// void berga(unsigned short instruction) {}


// Fx07 - LD Vx, DT
// Set Vx = delay timer value.
// void berga(unsigned short instruction) {}


// Fx0A - LD Vx, K
// Wait for a key press, store the value of the key in Vx.
// void berga(unsigned short instruction) {}


// Fx15 - LD DT, Vx
// Set delay timer = Vx.
// void berga(unsigned short instruction) {}


// Fx18 - LD ST, Vx
// Set sound timer = Vx.
// void berga(unsigned short instruction) {}


// Fx1E - ADD I, Vx
// Set I = I + Vx.
// void berga(unsigned short instruction) {}


// Fx29 - LD F, Vx
// Set I = location of sprite for digit Vx.
// void berga(unsigned short instruction) {}


// Fx33 - LD B, Vx
// Store BCD representation of Vx in memory locations I, I+1, and I+2.
// void berga(unsigned short instruction) {}


// Fx55 - LD [I], Vx
// Store registers V0 through Vx in memory starting at location I.
// void berga(unsigned short instruction) {}


// Fx65 - LD Vx, [I]
// Read registers V0 through Vx from memory starting at location I.
// void berga(unsigned short instruction) {}


void execute_instruction(unsigned short instruction) {

    // printf("executing instruction 0x%04x\n", instruction);
    // printf("first byte: 0x%x\n", instruction>>12);
    unsigned char least_significant_byte;

    switch (instruction>>12) {
        case 0x0:
            switch(instruction) {
                case 0x00E0:
                    instruction_clear_screen();
                    break;
                case 0x00EE:
                    // printf("DEBUG: return\n");
                    instruction_return_subroutine();
                    break;
            }
            break;
        case 0x1:
            // printf("DEBUG: jump\n");
            instruction_jump(instruction);
            break;
        case 0x2:
            // printf("DEBUG: call_subroutine\n");
            instruction_call_subroutine(instruction);
            break;
        case 0x3:
            instruction_skip_equal_byte(instruction);
            break;
        case 0x4:
            instruction_skip_not_equal_byte(instruction);
            break;
        case 0x5:
            instruction_skip_equal_reg(instruction);
            break;
        case 0x6:
            instruction_load_byte(instruction);
            break;
        case 0x7:
            instruction_add_byte(instruction);
            break;
        case 0x8:
            least_significant_byte = instruction & 0xF;
            switch (least_significant_byte) {
                case 0x0:
                    instruction_load_reg(instruction);
                    break;
                case 0x1:
                    instruction_or(instruction);
                    break;
                case 0x2:
                    instruction_and(instruction);
                    break;
                case 0x3:
                    instruction_xor(instruction);
                    break;
                case 0x4:
                    instruction_add_reg(instruction);
                    break;
                case 0x5:
                    instruction_sub_reg(instruction);
                    break;
                case 0x6:
                    instruction_shift_right(instruction);
                    break;
                case 0x7:
                    instruction_subn_reg(instruction);
                    break;
                case 0xE:
                    instruction_shift_left(instruction);
                    break;
                default:
                    break;
            }
            break;
        case 0x9:
            instruction_skip_not_equal_reg(instruction);
            break;
        case 0xA:
            break;
        case 0xB:
            break;
        case 0xC:
            break;
        case 0xD:
            break;
        case 0xE:
            break;
        case 0xF:
            break;


        
        default:        // invalid opcode
            // printf("invalid instruction...\n");
            break;
    }

}