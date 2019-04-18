#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>

#define SIZE_PROGRAM        3232    // num bytes, first 0x200 bytes are unused to mimic CHIP-8 system memory
#define SIZE_STACK          16      // num 16-bit values
#define SIZE_DISPLAY        32      // num bytes
#define NUM_REGISTERS       16      // num 16-bit registers

unsigned char program[SIZE_PROGRAM];
unsigned char display[SIZE_DISPLAY];
short stack[SIZE_STACK];
unsigned char stack_ptr;

unsigned char reg[16];
short reg_I;
unsigned char sound_timer;
unsigned char delay_timer;

short program_counter;

// helper functions
void dump_memory();
void dump_registers();
void dump_stack();

// stack functions
void push_stack(short address);
short pop_stack();

// program memory functions
short read_instruction(short address);

#endif // GLOBAL_H