#include "display.h"

void initialize_SDL()
{
    int pos_x = 100;
    int pos_y = 100;
    int win_display_w = RESOLUTION_X * PIXEL_SCALE;
    int win_display_h = RESOLUTION_Y * PIXEL_SCALE;
    int win_debug_w = 600;
    int win_debug_h = 480;

    // initialize SDL subsystems
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return;
    }

    if (TTF_Init() == -1)
    {
        printf("TTF_Init Error: %s\n", TTF_GetError());
        return;
    }

    // open a window for the debug
    win_debug = SDL_CreateWindow("dgchip8 DEBUG", pos_x + win_display_w, pos_y, win_debug_w, win_debug_h, SDL_WINDOW_SHOWN);
    if (win_debug == NULL)
    {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        quit_SDL();
        return;
    }

    // open a window for the display
    win_display = SDL_CreateWindow("dgchip8", pos_x, pos_y, win_display_w, win_display_h, SDL_WINDOW_SHOWN);
    if (win_display == NULL)
    {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        quit_SDL();
        return;
    }

    // create renderer for the debug
    ren_debug = SDL_CreateRenderer(win_debug, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren_debug == NULL)
    {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        quit_SDL();
        return;
    }

    // create renderer for the display
    ren_display = SDL_CreateRenderer(win_display, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren_display == NULL)
    {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        quit_SDL();
        return;
    }

    // load font
    font = TTF_OpenFont("fonts/SourceCodePro-Bold.ttf", 16);
    if (font == NULL)
    {
        printf("TTF_OpenFont Error: %s\n", TTF_GetError());
        quit_SDL();
        return;
    }

    fg_color = (SDL_Color){255, 255, 255, 255};
    bg_color = (SDL_Color){0, 0, 0, 255};
}

void update_screen_display()
{
    SDL_SetRenderDrawColor(ren_display, 0, 0, 0, 255);
    SDL_RenderClear(ren_display);
    SDL_SetRenderDrawColor(ren_display, 255, 255, 255, 255);

    SDL_Rect pixel;
    pixel.w = PIXEL_SCALE;
    pixel.h = PIXEL_SCALE;

    char current_display_byte;
    char masked;
    for (int i = 0; i < SIZE_DISPLAY; i++)
    {
        current_display_byte = display[i];
        for (int b = 0; b < 8; b++)
        {
            masked = (current_display_byte >> (7 - b)) & 0x1;
            pixel.x = (8 * i + b) % RESOLUTION_X * PIXEL_SCALE;
            pixel.y = (8 * i + b) / RESOLUTION_X * PIXEL_SCALE;

            if (masked)
            {
                SDL_RenderFillRect(ren_display, &pixel);
            }
        }
    }

    SDL_RenderPresent(ren_display);
}

void free_text_resources()
{
    SDL_DestroyTexture(text_texture);
    SDL_FreeSurface(text_surface);
}

void text_color_white()
{
    fg_color = (SDL_Color){255, 255, 255, 255};
}

void text_color_highlight()
{
    fg_color = (SDL_Color){0, 160, 255, 255};
}

void render_debug_text(const char *text, int x, int y)
{
    text_surface = TTF_RenderText(font, text, fg_color, bg_color);
    if (text_surface == NULL)
    {
        printf("TTF_RenderText Error: %s\n", TTF_GetError());
        free_text_resources();
        return;
    }

    text_texture = SDL_CreateTextureFromSurface(ren_debug, text_surface);
    if (text_surface == NULL)
    {
        printf("SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
        free_text_resources();
        return;
    }

    // printf("text_surface->h: %d\n", text_surface->h);
    text_rect = &(SDL_Rect){x, y, text_surface->w, text_surface->h};
    SDL_RenderCopy(ren_debug, text_texture, NULL, text_rect);

    free_text_resources();
}

void update_screen_debug()
{
    char text[50];

    SDL_SetRenderDrawColor(ren_display, 0, 0, 0, 255);
    SDL_RenderClear(ren_debug);
    SDL_SetRenderDrawColor(ren_display, 255, 255, 255, 255);

    // render register state
    text_color_white();
    render_debug_text("REGISTERS:", 10, 0);
    for (int i = 0; i < NUM_REGISTERS; i++)
    {
        sprintf(text, "REG_%x 0x%02x", i, reg[i]);
        render_debug_text(text, 20, 20 + i * 20);
    }
    sprintf(text, "REG_I 0x%04x", reg_I);
    render_debug_text(text, 20, 360);
    sprintf(text, "DELAY 0x%02x", delay_timer);
    render_debug_text(text, 20, 380);
    sprintf(text, "SOUND 0x%02x", sound_timer);
    render_debug_text(text, 20, 400);

    // render stack
    render_debug_text("STACK:", 160, 2);
    int cur_stack_index;
    for (int i = 0; i < SIZE_STACK; i++)
    {
        cur_stack_index = SIZE_STACK - 1 - i;
        if (stack_ptr == cur_stack_index)
        {
            text_color_highlight();
            sprintf(text, "0x%02x 0x%04x <-", cur_stack_index, stack[cur_stack_index]);
        }
        else
        {
            text_color_white();
            sprintf(text, "0x%02x 0x%04x", cur_stack_index, stack[cur_stack_index]);
        }
        render_debug_text(text, 170, 20 + i * 20);
    }

    // render program code at program counter
    int window = 2 * 11; // num of shorts to render on each side of the program counter's short
    int row = 0;
    text_color_white();
    render_debug_text("PROGRAM:", 340, 2);
    for (int i = program_counter - window; i < program_counter + window; i += 2)
    {
        // printf("i: %d\n", i);
        if (i == program_counter)
        {
            text_color_highlight();
            sprintf(text, "0x%04x 0x%04x [PC]", i, read_program_short(i));
        }
        else if (i < 0)
        {
            text_color_white();
            sprintf(text, "------ ------");
        }
        else
        {
            text_color_white();
            sprintf(text, "0x%04x 0x%04x", i, read_program_short(i));
        }
        render_debug_text(text, 350, 20 + row * 20);
        row++;
    }

    SDL_RenderPresent(ren_debug);
}

// void print_key_details(SDL_Event *e)
// {
//     printf("SDL_KEYDOWN Event    type:%d    timestamp:%d    state:%d    repeat:%d    keysym:%s\n", e->key.type, e->key.timestamp, e->key.state, e->key.repeat, e->key.keysym);
// }

// void handle_key_event(SDL_KeyboardEvent *k)
// {
//     if (e->type == SDLK_SPACE) {
//         printf("Spacebar detected!\n");
//     }
// }

void handle_SDL_events()
{
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT)
        {
            quit = 1;
        }
        else if (e.type == SDL_KEYDOWN)
        {
            printf("SDL_KEYDOWN Event    type:%d    timestamp:%d    state:%d    repeat:%d    keysym:%i\n", e.key.type, e.key.timestamp, e.key.state, e.key.repeat, e.key.keysym.sym);
            // print_key_details(&e);
            // handle_key_event(&e);
        }
    }
}

void quit_SDL()
{
    TTF_CloseFont(font);
    SDL_DestroyRenderer(ren_display);
    SDL_DestroyRenderer(ren_debug);
    SDL_DestroyWindow(win_display);
    SDL_DestroyWindow(win_debug);
    SDL_Quit();
}
