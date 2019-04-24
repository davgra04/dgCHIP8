#include "chip8.h"
#include "global.h"
#include "instruction.h"
#include "sdl_io.h"

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

void read_args(int argc, char** argv) {
  for (int i = 0; i < argc; i++) {
    if (strcmp(argv[i], "-f") == 0) {
      FREEZE_CLOCK = 1;
    }
  }

  strcpy(GAME_PATH, argv[argc - 1]);

  printf("FREEZE_CLOCK: %d\n", FREEZE_CLOCK);
  printf("GAME_PATH:    %s\n", GAME_PATH);
}

long get_time_diff(struct timeval* tv1, struct timeval* tv2) {
  return (tv2->tv_sec - tv1->tv_sec) * 1000000 + (tv2->tv_usec - tv1->tv_usec);
}

// main routine
int main(int argc, char** argv) {
  printf("Heyo Boyo\n");

  PIXEL_SCALE = 10;
  QUIT = 0;

  read_args(argc, argv);
  initialize_chip8();
  initialize_SDL();
  load_font_data("fonts/FONTSPRITE1");
  load_program(GAME_PATH);
  // dump_program();

  // set up timing vars
  struct timeval t_last_instruction, t_last_timer_decrement,
      t_last_screen_refresh, t_current, t_sub;
  long diff;
  long cycle_period = 1000000 / CHIP8_CLOCK;
  long timer_period = 1000000 / TIMER_DECREMENT_CLOCK;
  long display_period = 1000000 / SCREEN_REFRESH_CLOCK;
  int num_instructions, num_decrements;

  gettimeofday(&t_last_instruction, NULL);
  gettimeofday(&t_last_timer_decrement, NULL);
  gettimeofday(&t_last_screen_refresh, NULL);
  gettimeofday(&t_current, NULL);

  t_sub.tv_sec = 0;
  t_sub.tv_usec = 0;

  // TODO: consider executing multiple instructions per loop based on timing
  while (!QUIT) {
    // Execute instruction(s)
    gettimeofday(&t_current, NULL);
    diff = get_time_diff(&t_last_instruction, &t_current);
    if (diff > cycle_period) {
      poll_SDL_events();

      // execute instructions
      num_instructions = diff / cycle_period;
      step_emulation(num_instructions);

      // set time of last instruction
      gettimeofday(&t_last_instruction, NULL);

      // offset time of last instruction with leftover time from diff
      t_sub.tv_usec = diff % cycle_period;
      timersub(&t_last_instruction, &t_sub, &t_last_instruction);
    }

    // Decrement timers
    gettimeofday(&t_current, NULL);
    diff = get_time_diff(&t_last_timer_decrement, &t_current);
    if (diff > timer_period) {
      // decrement timers
      num_decrements = diff / timer_period;
      decrement_timers(num_decrements);

      // set time of last timer decrement
      gettimeofday(&t_last_timer_decrement, NULL);

      // offset time of timer decrement with leftover time from diff
      t_sub.tv_usec = diff % cycle_period;
      timersub(&t_last_timer_decrement, &t_sub, &t_last_timer_decrement);
    }

    // Redraw screen
    gettimeofday(&t_current, NULL);
    diff = get_time_diff(&t_last_screen_refresh, &t_current);
    if (diff > display_period) {
      // redraw
      update_screen_display();
      // update_screen_debug();

      // set time of last screen draw
      gettimeofday(&t_last_screen_refresh, NULL);

      // offset time of screen draw with leftover time from diff
      t_sub.tv_usec = diff % cycle_period;
      timersub(&t_last_screen_refresh, &t_sub, &t_last_screen_refresh);
    }
  }

  quit_SDL();

  return 0;
}
