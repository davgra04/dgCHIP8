#include "sdl_io.h"

// initialization
////////////////////////////////////////////////////////////////////////////////

int win_pos_x, win_pos_y, win_display_w, win_display_h, win_debug_w,
    win_debug_h;

char* title_base = "dgchip8 - ";
char* title;

void initialize_SDL_subsystems() {
  win_pos_x = 20;
  win_pos_y = 20;
  win_display_w = RESOLUTION_X * PIXEL_SCALE;
  win_display_h = RESOLUTION_Y * PIXEL_SCALE;
  win_debug_w = 600;
  win_debug_h = 480;

  // build title
  if ((title = malloc(strlen(title_base) + strlen(GAME_PATH) + 1)) != NULL) {
    title[0] = '\0';  // ensures the memory is an empty string
    strcat(title, title_base);
    strcat(title, GAME_PATH);
  } else {
    printf("malloc failed!\n");
    exit(1);
  }

  // initialize SDL subsystems
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
    printf("SDL_Init Error: %s\n", SDL_GetError());
    return;
  }

  if (TTF_Init() == -1) {
    printf("TTF_Init Error: %s\n", TTF_GetError());
    return;
  }

  // initialize sdl mixer, open up the audio device
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0) {
    printf("Mix_OpenAudio Error: %s\n", Mix_GetError());
    return;
  }
}

void create_display_window() {
  // open a window for the display
  win_display = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, win_display_w,
                                 win_display_h, SDL_WINDOW_SHOWN);
  if (win_display == NULL) {
    printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
    quit_SDL();
    return;
  }

  // create renderer for the display
  ren_display = SDL_CreateRenderer(win_display, -1, SDL_RENDERER_ACCELERATED);
  //   win_display, -1, SDL_RENDERER_ACCELERATED |
  //   SDL_RENDERER_PRESENTVSYNC);
  if (ren_display == NULL) {
    printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
    quit_SDL();
    return;
  }
}

void create_debug_window() {
  // open a window for the debug
  win_debug =
      SDL_CreateWindow("dgchip8 DEBUG", win_pos_x + win_display_w, win_pos_y,
                       win_debug_w, win_debug_h, SDL_WINDOW_SHOWN);
  if (win_debug == NULL) {
    printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
    quit_SDL();
    return;
  }

  // create renderer for the debug
  ren_debug = SDL_CreateRenderer(win_debug, -1, SDL_RENDERER_ACCELERATED);
  //   win_debug, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (ren_debug == NULL) {
    printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
    quit_SDL();
    return;
  }
}

void initialize_SDL() {
  initialize_SDL_subsystems();
  create_display_window();
  // create_debug_window();

  // load font
  font = TTF_OpenFont("fonts/SourceCodePro-Bold.ttf", 16);
  if (font == NULL) {
    printf("TTF_OpenFont Error: %s\n", TTF_GetError());
    quit_SDL();
    return;
  }

  // load beep
  beep = Mix_LoadWAV("sounds/beep.wav");

  for (int i = 0; i < NUM_BUTTONS; i++) {
    button_state[i] = 0;
  }

  text_rect = &(SDL_Rect){0, 0, 0, 0};

  fg_color = (SDL_Color){255, 255, 255, 255};
  bg_color = (SDL_Color){0, 0, 0, 255};
}

// beep sound
////////////////////////////////////////////////////////////////////////////////
int beep_playing;

void start_beep() {
  if (!beep_playing) {
    if (Mix_PlayChannel(-1, beep, 0) < 0) {
      printf("Error playing beep\n");
      return;
    }
    beep_playing = 1;
  }
}

void stop_beep() {
  if (beep_playing) {
    Mix_HaltChannel(-1);
    beep_playing = 0;
  }
}

// screen drawing
////////////////////////////////////////////////////////////////////////////////

char current_display_byte;
char masked;

void update_screen_display() {
  SDL_SetRenderDrawColor(ren_display, 0, 0, 0, 255);
  SDL_RenderClear(ren_display);
  SDL_SetRenderDrawColor(ren_display, 255, 255, 255, 255);

  SDL_Rect pixel;
  pixel.w = PIXEL_SCALE;
  pixel.h = PIXEL_SCALE;

  // draw pixels
  for (int i = 0; i < SIZE_DISPLAY; i++) {
    current_display_byte = display[i];
    for (int b = 0; b < 8; b++) {
      masked = (current_display_byte >> (7 - b)) & 0x1;
      pixel.x = (8 * i + b) % RESOLUTION_X * PIXEL_SCALE;
      pixel.y = (8 * i + b) / RESOLUTION_X * PIXEL_SCALE;

      if (masked) {
        SDL_RenderFillRect(ren_display, &pixel);
      }
    }
  }

  // // draw pixel grid
  // SDL_SetRenderDrawColor(ren_display, 0, 160, 255, 255);
  // for (int i = 0; i < SIZE_DISPLAY * 8; i++)
  // {
  //     pixel.x = i % RESOLUTION_X * PIXEL_SCALE;
  //     pixel.y = i / RESOLUTION_X * PIXEL_SCALE;
  //     SDL_RenderDrawRect(ren_display, &pixel);
  // }

  SDL_RenderPresent(ren_display);
}

void free_text_resources() {
  SDL_DestroyTexture(text_texture);
  SDL_FreeSurface(text_surface);
}

void text_color_white() { fg_color = (SDL_Color){255, 255, 255, 255}; }

void text_color_highlight() { fg_color = (SDL_Color){0, 160, 255, 255}; }

void render_debug_text(const char* text, int x, int y) {
  text_surface = TTF_RenderText_Solid(font, text, fg_color);
  if (text_surface == NULL) {
    printf("TTF_RenderText Error: %s\n", TTF_GetError());
    free_text_resources();
    return;
  }

  text_texture = SDL_CreateTextureFromSurface(ren_debug, text_surface);
  if (text_surface == NULL) {
    printf("SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
    free_text_resources();
    return;
  }

  // printf("text_surface->h: %d\n", text_surface->h);
  // text_rect = &(SDL_Rect){x, y, text_surface->w, text_surface->h};
  text_rect->x = x;
  text_rect->y = y;
  text_rect->w = text_surface->w;
  text_rect->h = text_surface->h;
  SDL_RenderCopy(ren_debug, text_texture, NULL, text_rect);

  free_text_resources();
}

int window, row, cur_stack_index;

void update_screen_debug() {
  char text[50];

  SDL_SetRenderDrawColor(ren_display, 0, 0, 0, 255);
  SDL_RenderClear(ren_debug);
  SDL_SetRenderDrawColor(ren_display, 255, 255, 255, 255);

  // render register state
  text_color_white();
  render_debug_text("REGISTERS:", 10, 0);
  for (int i = 0; i < NUM_REGISTERS; i++) {
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
  for (int i = 0; i < SIZE_STACK; i++) {
    cur_stack_index = SIZE_STACK - 1 - i;
    if (stack_ptr == cur_stack_index) {
      text_color_highlight();
      sprintf(text, "0x%02x 0x%04x <-", cur_stack_index,
              stack[cur_stack_index]);
    } else {
      text_color_white();
      sprintf(text, "0x%02x 0x%04x", cur_stack_index, stack[cur_stack_index]);
    }
    render_debug_text(text, 170, 20 + i * 20);
  }

  // render program code at program counter
  window = 2 * 11;  // num of shorts to render on each side of the program
                    // counter's short
  row = 0;
  text_color_white();
  render_debug_text("PROGRAM:", 340, 2);
  for (int i = program_counter - window; i < program_counter + window; i += 2) {
    // printf("i: %d\n", i);
    if (i == program_counter) {
      text_color_highlight();
      sprintf(text, "0x%04x 0x%04x [PC]", i, read_program_short(i));
    } else if (i < 0) {
      text_color_white();
      sprintf(text, "------ ------");
    } else {
      text_color_white();
      sprintf(text, "0x%04x 0x%04x", i, read_program_short(i));
    }
    render_debug_text(text, 350, 20 + row * 20);
    row++;
  }

  SDL_RenderPresent(ren_debug);
}

// run emulation
////////////////////////////////////////////////////////////////////////////////

int chip8_key_to_sdl_key(int chip8_key) {
  switch (chip8_key) {
    case 0x1:
      return SDLK_1;
    case 0x2:
      return SDLK_2;
    case 0x3:
      return SDLK_3;
    case 0xC:
      return SDLK_4;
    case 0x4:
      return SDLK_q;
    case 0x5:
      return SDLK_w;
    case 0x6:
      return SDLK_e;
    case 0xD:
      return SDLK_r;
    case 0x7:
      return SDLK_a;
    case 0x8:
      return SDLK_s;
    case 0x9:
      return SDLK_d;
    case 0xE:
      return SDLK_f;
    case 0xA:
      return SDLK_z;
    case 0x0:
      return SDLK_x;
    case 0xB:
      return SDLK_c;
    case 0xF:
      return SDLK_v;
    default:
      return -1;
  }
}

int sdl_key_to_chip8_key(int sdl_key) {
  switch (sdl_key) {
    case SDLK_1:
      return 0x1;
    case SDLK_2:
      return 0x2;
    case SDLK_3:
      return 0x3;
    case SDLK_4:
      return 0xC;
    case SDLK_q:
      return 0x4;
    case SDLK_w:
      return 0x5;
    case SDLK_e:
      return 0x6;
    case SDLK_r:
      return 0xD;
    case SDLK_a:
      return 0x7;
    case SDLK_s:
      return 0x8;
    case SDLK_d:
      return 0x9;
    case SDLK_f:
      return 0xE;
    case SDLK_z:
      return 0xA;
    case SDLK_x:
      return 0x0;
    case SDLK_c:
      return 0xB;
    case SDLK_v:
      return 0xF;
    default:
      return -1;
  }
}

void poll_SDL_events() {
  // reset keydown states
  for (int i = 0; i < NUM_BUTTONS; i++) {
    button_keydown[i] = 0;
  }

  while (SDL_PollEvent(&e)) {
    switch (e.type) {
      case SDL_QUIT:
        QUIT = 1;
        break;
      case SDL_KEYDOWN:
        // printf("KEYDOWN: %d\n", e.key.keysym.sym);
        if (e.key.keysym.sym < NUM_BUTTONS) {
          button_state[e.key.keysym.sym] = 1;
          button_keydown[e.key.keysym.sym] = 1;
        }
        break;
      case SDL_KEYUP:
        if (e.key.keysym.sym < NUM_BUTTONS) button_state[e.key.keysym.sym] = 0;
        break;
      default:
        break;
    }
  }
}

void quit_SDL() {
  printf("Closing SDL resources\n");
  Mix_FreeChunk(beep);
  Mix_CloseAudio();
  TTF_CloseFont(font);
  TTF_Quit();
  SDL_DestroyRenderer(ren_display);
  SDL_DestroyRenderer(ren_debug);
  SDL_DestroyWindow(win_display);
  SDL_DestroyWindow(win_debug);
  SDL_Quit();
}
