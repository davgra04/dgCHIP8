#include "global.h"
#include "chip8.h"
#include "instruction.h"
#include "sdlio.h"
// #include <SDL2/SDL.h>

void read_args(int argc, char **argv)
{

    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-f") == 0)
        {
            FREEZE_CLOCK = 1;
        }
    }

    strcpy(GAME_PATH, argv[argc - 1]);

    printf("FREEZE_CLOCK: %d\n", FREEZE_CLOCK);
    printf("GAME_PATH:    %s\n", GAME_PATH);
}

// main routine
int main(int argc, char **argv)
{
    printf("Heyo Boyo\n");

    PIXEL_SCALE = 8;
    QUIT = 0;

    read_args(argc, argv);
    initialize_chip8();
    initialize_SDL();
    load_program(GAME_PATH);

    while (!quit)
    {
        run_emulation();
    }

    quit_SDL();

    return 0;
}
