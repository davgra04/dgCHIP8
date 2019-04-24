#include "chip8.h"

// helper functions

void clear_program() {
  for (int i = 0; i < SIZE_PROGRAM; i++) {
    program[i] = 0x00;
  }
}

void clear_display() {
  for (int i = 0; i < SIZE_DISPLAY; i++) {
    display[i] = 0x00;
  }
}

void checkerboard_display() {
  for (int i = 0; i < SIZE_DISPLAY; i++) {
    // display[i] = 0x55;

    if ((i * 8 / RESOLUTION_X) % 2 == 0) {
      display[i] = 0x55;
    } else {
      display[i] = 0xAA;
    }
  }
}

void random_display() {
  for (int i = 0; i < SIZE_DISPLAY; i++) {
    display[i] = rand() & 0xFF;

    // if((i*8/RESOLUTION_X)%2 == 0) {
    //     display[i] = 0x55;
    // } else {
    //     display[i] = 0xAA;
    // }
  }
}

void clear_stack() {
  for (int i = 0; i < SIZE_STACK; i++) {
    stack[i] = 0x00;
  }
  stack_ptr = 0x0;
}

void clear_registers() {
  for (int i = 0; i < NUM_REGISTERS; i++) {
    reg[i] = 0x00;
  }
  reg_I = 0x0;
  sound_timer = 0x0;
  delay_timer = 0x0;
}

void initialize_chip8() {
  clear_program();
  clear_stack();
  clear_display();
  clear_registers();

  srand(time(NULL));        // set seed for RNG
  program_counter = 0x200;  // set program counter to start of program
}

void dump_program() {
  printf("Program:\n");
  for (int i = 0; i < SIZE_PROGRAM; i += 2) {
    printf("  0x%04x    0x%02x%02x\n", i, program[i], program[i + 1]);
  }
}

void dump_display() {
  printf("Display:\n");
  for (int i = 0; i < SIZE_DISPLAY; i++) {
    printf("  0x%04x    0x%02x\n", i, (unsigned int)display[i]);
  }
}

void dump_registers() {
  printf("Registers:\n");
  for (int i = 0; i < 16; i++) {
    printf("  0x%01x       0x%04x\n", i, (unsigned int)reg[i]);
  }
  printf("  I         0x%08x\n", reg_I);
  printf("  sound     0x%04x\n", sound_timer);
  printf("  delay     0x%04x\n", delay_timer);
}

void dump_stack() {
  printf("Stack:\n");
  for (int i = SIZE_STACK - 1; i > -1; i--) {
    printf("  0x%x       0x%08x", i, stack[i]);
    if ((unsigned char)i == stack_ptr) {
      printf("  <---- stack_ptr\n");
    } else {
      printf("\n");
    }
  }
  printf("\n");
  printf("  stack_ptr: 0x%x\n", stack_ptr);
}

// stack functions

void push_stack(unsigned short address) {
  if (stack_ptr < SIZE_STACK) {
    stack[stack_ptr] = address;
    stack_ptr++;
  }
}

unsigned short pop_stack() {
  if (stack_ptr > 0) {
    stack_ptr--;
    return stack[stack_ptr];
  }
  return 0xFF;
}

// memory access functions

unsigned char read_program_byte(unsigned short address) {
  return program[address];
}

unsigned short read_program_short(unsigned short address) {
  return (program[address] << 8) + program[address + 1];
}

unsigned char read_display_byte(unsigned short address) {
  return display[address];
}

unsigned short read_display_short(unsigned short address) {
  return (display[address % SIZE_DISPLAY] << 8) +
         display[(address + 1) % SIZE_DISPLAY];
}

void write_display_short(unsigned short address, unsigned short value) {
  display[address % SIZE_DISPLAY] = (value >> 0x8) & 0xFF;
  display[(address + 1) % SIZE_DISPLAY] = value & 0xFF;
}

void load_program(char *path) {
  FILE *f = fopen(path, "rb");
  if (f == NULL) {
    printf("failed to open path: %s\n", path);
    exit(1);
  }

  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  fseek(f, 0, SEEK_SET);

  if (fsize > SIZE_PROGRAM - 0x200) {
    printf("Cannot load program, too big!\n");
    exit(1);
  }

  fread(program + 0x200, 1, fsize, f);
  fclose(f);
}

void load_font_data(char *path) {
  FILE *f = fopen(path, "rb");
  if (f == NULL) {
    printf("failed to open path: %s\n", path);
    exit(1);
  }

  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  fseek(f, 0, SEEK_SET);

  if (fsize > 0x200) {
    printf("Cannot load font data, too big!\n");
    exit(1);
  }

  fread(program, 1, fsize, f);
  fclose(f);
}

void step_emulation(int num_steps) {

  // printf("Executing %d steps\n", num_steps);

  for (int i = 0; i < num_steps; i++) {
    unsigned short instruction = read_program_short(program_counter);
    program_counter += 2;
    // printf("executing instruction 0x%04x...\n", instruction);
    execute_instruction(instruction);
  }
}

void decrement_timers(int num_decrements) {
  for (int i = 0; i < num_decrements; i++) {
    if (sound_timer > 0) {
      sound_timer--;
      if (sound_timer == 0) {
        stop_beep();
      }
    }
    if (delay_timer > 0) {
      delay_timer--;
    }
  }
}
