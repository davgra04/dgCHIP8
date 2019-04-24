#include "chip8.h"
#include "global.h"
#include "instruction.h"
#include "sdl_io.h"

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

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

  PIXEL_SCALE = 10;
  QUIT = 0;

  read_args(argc, argv);
  initialize_chip8();
  initialize_SDL();
  load_font_data("fonts/FONTSPRITE1");
  load_program(GAME_PATH);

  // dump_program();
  // exit(0);

  // set up timing vars
  struct timeval tv1, tv2;
  long diff, slpamt;
  long cycle_period = 1000000 / CHIP8_CLOCK;
  int cycles_per_display = CHIP8_CLOCK / SCREEN_REFRESH_CLOCK;
  int cycles_per_timer_dec = CHIP8_CLOCK / TIMER_DECREMENT_CLOCK;
  // long cycle_period = 1000000;
  // int cycles_per_display = 1;
  // int cycles_per_timer_dec = 1;

  int i = 0;

  // TODO: consider executing multiple instructions per loop based on timing
  while (!QUIT) {
    gettimeofday(&tv1, NULL);

    poll_SDL_events();
    step_emulation();
    if (i % cycles_per_display == 0) {
      update_screen_display();
      update_screen_debug();
    }
    if (i % cycles_per_timer_dec == 0) {
      decrement_timers();
    }

    // sleep until end of cycle
    gettimeofday(&tv2, NULL);
    diff = (tv2.tv_sec - tv1.tv_sec) * 1000000 + (tv2.tv_usec - tv1.tv_usec);
    slpamt = cycle_period - diff;
    // printf("diff: %ld\n", diff);
    if (slpamt > 0) {
      // printf("    sleeping: %ld\n", slpamt);
      usleep(slpamt);
    }

    i++;
  }

  quit_SDL();

  return 0;
}
