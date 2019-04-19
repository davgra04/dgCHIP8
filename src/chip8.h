#ifndef CHIP8_H
#define CHIP8_H

#include <stdio.h>
#include <stdlib.h>
#include "time.h"

#define SIZE_PROGRAM        3232    // num bytes, first 0x200 bytes are unused to mimic CHIP-8 system memory
#define SIZE_STACK          16      // num 16-bit values
#define SIZE_DISPLAY        256     // num bytes
#define NUM_REGISTERS       16      // num 16-bit registers
#define RESOLUTION_X        64
#define RESOLUTION_Y        32

unsigned char program[SIZE_PROGRAM];
unsigned char display[SIZE_DISPLAY];
unsigned short stack[SIZE_STACK];
unsigned char stack_ptr;

unsigned char reg[16];
unsigned short reg_I;
unsigned char sound_timer;
unsigned char delay_timer;

unsigned short program_counter;

int quit;

// helper functions
void clear_program();
void clear_display();
void clear_stack();
void clear_registers();
void initialize_chip8();

void checkerboard_display();
void random_display();

void dump_program();
void dump_display();
void dump_registers();
void dump_stack();

// stack functions
void push_stack(unsigned short address);
unsigned short pop_stack();

// program memory functions
unsigned char read_display_byte(unsigned short address);
unsigned char read_program_byte(unsigned short address);
unsigned short read_program_short(unsigned short address);

void load_program(char* path);

#endif // CHIP8_H