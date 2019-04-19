#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "global.h"
#include "chip8.h"

// dummy functions
int return_four();

void instruction_clear_screen();
void instruction_return_subroutine();
void instruction_jump(unsigned short instruction);
void instruction_call_subroutine(unsigned short instruction);
void instruction_skip_equal_byte(unsigned short instruction);
void instruction_skip_not_equal_byte(unsigned short instruction);
void instruction_skip_equal_reg(unsigned short instruction);
void instruction_load_byte(unsigned short instruction);
void instruction_add_byte(unsigned short instruction);
void instruction_load_reg(unsigned short instruction);
void instruction_or(unsigned short instruction);
void instruction_and(unsigned short instruction);
void instruction_xor(unsigned short instruction);
void instruction_add_reg(unsigned short instruction);
void instruction_sub_reg(unsigned short instruction);
void instruction_shift_right(unsigned short instruction);
void instruction_subn_reg(unsigned short instruction);
void instruction_shift_left(unsigned short instruction);
void instruction_skip_not_equal_reg(unsigned short instruction);
void instruction_load_regI(unsigned short instruction);
void instruction_jump_reg(unsigned short instruction);
void instruction_rand(unsigned short instruction);
void instruction_draw_sprite(unsigned short instruction);


// executes an instruction
void execute_instruction(unsigned short instruction);


#endif // INSTRUCTION_H