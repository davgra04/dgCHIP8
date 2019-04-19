#include "global.h"
#include "chip8.h"
#include "instruction.h"
#include "display.h"
// #include <SDL2/SDL.h>

// main routine
int main()
{
    printf("Heyo Boyo\n");

    PIXEL_SCALE = 8;
    QUIT = 0;

    // dump_program();   //print all memory values
    // dump_registers(); //print all register values

    initialize_chip8();
    initialize_SDL();

    checkerboard_display();
    dump_display();

    load_program("games/TETRIS");

    while (!quit)
    {
        // step_emulation();
        // random_display();
        update_screen_display();
        update_screen_debug();
        handle_SDL_events();
    }

    quit_SDL();

    return 0;
}
