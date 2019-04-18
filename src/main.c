#include "global.h"
#include "instruction.h"

// main routine
int main() {
    printf("Heyo Boyo\n");

    //print all memory values
    dump_memory();

    //print all register values
    dump_registers();


    return 0;
}
