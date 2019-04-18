#include "minunit.h"
#include <stdbool.h>
#include <stdlib.h>
#include "../src/global.h"
#include "../src/instruction.h"

int tests_run = 0;


// HELPER FUNCTIONS

void clear_program() {
    for(int i=0; i<SIZE_PROGRAM; i++) {
        program[i] = 0x00;
    }
}

void clear_stack() {
    for(int i=0; i<SIZE_STACK; i++) {
        stack[i] = 0x00;
    }
    stack_ptr = 0x0;
}

void clear_registers() {
    for(int i=0; i<NUM_REGISTERS; i++) {
        reg[i] = 0x00;
    }
    reg_I = 0x0;
    sound_timer = 0x0;
    delay_timer = 0x0;
}

void initialize_system() {
    clear_program();
    clear_stack();
    instruction_clear_screen();
    clear_registers();

    program_counter = 0x200;
}

void write_short_to_program(short address, short value) {
    program[address] = (value >> 8) & 0xff;
    program[address + 1] = value & 0xff;
}

// STACK TESTS

static char* test_stack() {

    // push values to stack
    for(int i=0; i<20; i++) {
        // dump_stack();
        push_stack((unsigned char)i);
    }
    mu_assert("test_stack failed, stack_ptr incremented past 0x10", stack_ptr == (unsigned char)0x10);
    // dump_stack();

    // pop values from stack
    unsigned char popped;
    for(int i=15; i>-1; i--) {
        popped = pop_stack();
        mu_assert("test_stack failed, did not recieve correct value on pop", popped == (unsigned char)i);
        // dump_stack();
        // printf("popped value: 0x%08x\n", popped);
    }

    return 0;

}


// INSTRUCTION TESTS

static char* test_return_four() {
    int ret = return_four();
    mu_assert("error, return is not 4", ret == 4);
    return 0;
}

static char* test_display_clear() {
    // fill display memory with ones
    for (int i=0; i<SIZE_DISPLAY; i++) {
        display[i] = 0xFF;
    }

    // clear display
    execute_instruction(0x00E0);

    // check that all display memory is zeroed
    bool failed = false;
    for (int i=0; i<SIZE_DISPLAY; i++) {
        if(display[i] != 0x00) {
            failed = true;
        }
    }

    mu_assert("test_display_clear failed, nonzero value found in display memory", failed == false);
    return 0;
}

static char* test_call_jump_return() {
    // Load simple program that calls a subroutine at 0x20, which jumps to 0x40, and returns
    initialize_system();
    write_short_to_program(0x200, 0x2220);      // call subroutine at addr 0x220
    write_short_to_program(0x220, 0x1240);      // jump to addr 0x240
    write_short_to_program(0x240, 0x00EE);      // return from subroutine
    // dump_program();


    short instruction;

    instruction = read_instruction(program_counter++);
    printf("executing instruction 0x%04x...\n", instruction);
    execute_instruction(instruction);
    // dump_stack();
    // printf("program_counter 0x%04x\n", program_counter);
    mu_assert("test_call_jump_return failed, subroutine instruction: bad program_counter value", program_counter == 0x220);
    mu_assert("test_call_jump_return failed, subroutine instruction: bad first stack value", stack[0] == 0x201);
    // exit(0);

    instruction = read_instruction(program_counter++);
    printf("executing instruction 0x%04x...\n", instruction);
    execute_instruction(instruction);
    mu_assert("test_call_jump_return failed, jump instruction: bad program_counter value", program_counter == 0x240);

    instruction = read_instruction(program_counter++);
    printf("executing instruction 0x%04x...\n", instruction);
    execute_instruction(instruction);
    mu_assert("test_call_jump_return failed, jump instruction: bad program_counter value", program_counter == 0x201);

    return 0;
}


static char* all_tests() {
    // stack tests
    mu_run_test(test_stack);

    //instruction tests
    mu_run_test(test_return_four);
    mu_run_test(test_display_clear);
    mu_run_test(test_call_jump_return);

    return 0;
}






int main() {

    char *result = all_tests();
    if (result != 0) {
        printf("%s\n", result);
    }
    else {
        printf("ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);

    return result != 0;

}