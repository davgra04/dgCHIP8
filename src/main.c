#include "chip8.h"
#include "global.h"
#include "instruction.h"
#include "sdlio.h"

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
// #include <SDL2/SDL.h>

void read_args(int argc, char **argv) {
  for (int i = 0; i < argc; i++) {
    if (strcmp(argv[i], "-f") == 0) {
      FREEZE_CLOCK = 1;
    }
  }

  strcpy(GAME_PATH, argv[argc - 1]);

  printf("FREEZE_CLOCK: %d\n", FREEZE_CLOCK);
  printf("GAME_PATH:    %s\n", GAME_PATH);
}

// main routine
int main(int argc, char **argv) {
  printf("Heyo Boyo\n");

  PIXEL_SCALE = 8;
  QUIT = 0;

  read_args(argc, argv);
  initialize_chip8();
  initialize_SDL();
  load_program(GAME_PATH);

  // while (!quit)
  // {
  //     run_emulation();
  // }

  struct timeval tv1, tv2;
  gettimeofday(&tv1, NULL);
  printf("tv1.tv_sec:  %ld\n", tv1.tv_sec);
  printf("tv1.tv_usec: %d\n", tv1.tv_usec);

  long diff, slpamt;
  long cycle_period = 2000;  // usec
  // long cycle_period = 50000;  // usec

  SDL_Event e;
  int i=0;

  while (!QUIT) {
    // for (int i = 0; i < NUM_TRIALS; i++) {
    gettimeofday(&tv1, NULL);
    // printf("tv1: %lds %dus\n", tv1.tv_sec, tv1.tv_usec);
    //
    //  DO CALCULATION
    //

    step_emulation();
    if (i % 8 == 0) {
      while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
          QUIT = 1;
        }
      }
      update_screen_display();
      update_screen_debug();
    }
    //
    //
    gettimeofday(&tv2, NULL);
    // printf("tv2: %lds %dus\n", tv2.tv_sec, tv2.tv_usec);
    diff = (tv2.tv_sec - tv1.tv_sec) * 1000000 + (tv2.tv_usec - tv1.tv_usec);
    slpamt = cycle_period - diff;
    printf("diff: %ld\n\n", diff);
    if (slpamt > 0) {
      printf("sleeping: %ld\n\n", slpamt);
      usleep(slpamt);
    }

    i++;
  }

  usleep(1000000);

  quit_SDL();

  return 0;
}
