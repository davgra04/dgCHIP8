#include "global.h"

unsigned char stack_ptr = 0;
short reg_I = 0;
short program_counter = 0;

// helper functions

void dump_program() {
    printf("Program:\n");
    for(int i=0; i<SIZE_PROGRAM; i+=2) {
        printf("  0x%04x    0x%02x%02x\n", i, program[i], program[i+1]);
    }
}

void dump_display() {
    printf("Display:\n");
    for(int i=0; i<SIZE_DISPLAY; i++) {
        printf("  0x%04x    0x%02x\n", i, (unsigned int) display[i]);
    }
}

void dump_registers() {
    printf("Registers:\n");
    for(int i=0; i<16; i++) {
        printf("  0x%01x       0x%04x\n", i, (unsigned int) reg[i]);
    }
    printf("  I         0x%08x\n", reg_I);
    printf("  sound     0x%08x\n", sound_timer);
    printf("  delay     0x%08x\n", delay_timer);
}

void dump_stack() {
    printf("Stack:\n");
    for(int i=SIZE_STACK-1; i>-1; i--) {
        printf("  0x%x       0x%08x", i, stack[i]);
        if((unsigned char)i == stack_ptr) {
            printf("  <---- stack_ptr\n");
        }
        else {
            printf("\n");
        }
    }
    printf("\n");
    printf("  stack_ptr: 0x%x\n", stack_ptr);
}


// stack functions

void push_stack(short address) {
    if (stack_ptr < SIZE_STACK) {
        stack[stack_ptr] = address;
        stack_ptr++;
    }
}

short pop_stack() {
    if (stack_ptr > 0) {
        stack_ptr--;
        return stack[stack_ptr];
    }
}


// program memory functions

short read_instruction(short address) {
    return (program[address]<<8) + program[address+1];
}
