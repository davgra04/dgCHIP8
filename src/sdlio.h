#ifndef DISPLAY_H
#define DISPLAY_H

#include "global.h"
#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

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

SDL_TimerID emulation_timer;
SDL_TimerID display_timer;

SDL_Event e;
// initialization
////////////////////////////////////////////////////////////////////////////////

void initialize_SDL_subsystems();
void initialize_SDL_timers();
void initialize_SDL();

// timer callbacks
////////////////////////////////////////////////////////////////////////////////

Uint32 emulation_step_callback(Uint32 interval, void *param);
Uint32 display_callback(Uint32 interval, void *param);

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

void run_emulation();
void quit_SDL();

#endif // DISPLAY_H