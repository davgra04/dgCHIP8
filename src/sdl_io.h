#ifndef SDL_IO_H
#define SDL_IO_H

#include "global.h"
#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#define NUM_BUTTONS 128

SDL_Window *win_display;
SDL_Renderer *ren_display;
SDL_Window *win_debug;
SDL_Renderer *ren_debug;

SDL_Color fg_color;
SDL_Color hi_color;
SDL_Color bg_color;

SDL_Surface *text_surface;
SDL_Texture *text_texture;
SDL_Rect *text_rect;
TTF_Font *font;

Mix_Chunk* beep;

SDL_TimerID emulation_timer;
SDL_TimerID display_timer;

SDL_Event e;

int button_state[NUM_BUTTONS];
int button_keydown[NUM_BUTTONS];

// initialization
////////////////////////////////////////////////////////////////////////////////

void initialize_SDL_subsystems();
void initialize_SDL();

// beep sound
////////////////////////////////////////////////////////////////////////////////
void start_beep();
void stop_beep();

// screen drawing
////////////////////////////////////////////////////////////////////////////////

void update_screen_display();
void free_text_resources();
void text_color_white();
void text_color_highlight();
void render_debug_text(const char *text, int x, int y);
void update_screen_debug();

// run emulation
////////////////////////////////////////////////////////////////////////////////

int chip8_key_to_sdl_key(int chip8_key);
int sdl_key_to_chip8_key(int sdl_key);
void poll_SDL_events();
void quit_SDL();

#endif // SDL_IO_H