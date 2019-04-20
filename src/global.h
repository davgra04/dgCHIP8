#ifndef GLOBAL_H
#define GLOBAL_H

int PIXEL_SCALE;      // width/height of each rendered pixel in px
int QUIT;             // exits emulation when flag is set
char GAME_PATH[128];  // path of the game file to load
int FREEZE_CLOCK;     // if true, execution of instructions happens on spacebar

#endif  // GLOBAL_H