#ifndef DISPLAY_H
#define DISPLAY_H

#include "global.h"
#include "chip8.h"
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

SDL_Event e;

void initialize_SDL();
void update_screen_display();
void update_screen_debug();
void handle_SDL_events();
void quit_SDL();

#endif // DISPLAY_H