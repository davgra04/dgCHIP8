dgCHIP8
=======

A CHIP-8 emulator written in C. Includes some public domain CHIP-8 games in the `games/` directory, and a basic decompiler written in Python under the `tools/` directory.

![](tetris.gif)
![](invaders.gif)

## Libraries

* SDL2 - Graphics, sound, and input
* MinUnit - Unit testing

## Building (on macOS)

```bash
# install dependencies
brew install sdl2 sdl2_ttf sdl2_mixer

# build
cd dgCHIP8/
make clean all

# run
./dgCHIP8 <path/to/game>

```

## Controls

The CHIP-8 was designed for a 16-key hexadecimal keypad. This emulator uses the following mapping to a QWERTY keyboard:

```
 Keypad                  QWERTY
 --- --- --- ---         --- --- --- ---
| 1 | 2 | 3 | C |       | 1 | 2 | 3 | 4 |
 --- --- --- ---         --- --- --- ---
| 4 | 5 | 6 | D |       | Q | W | E | R |
 --- --- --- ---    ->   --- --- --- ---
| 7 | 8 | 9 | E |       | A | S | D | F |
 --- --- --- ---         --- --- --- ---
| A | 0 | B | F |       | Z | X | C | V |
 --- --- --- ---         --- --- --- ---
```

## Running the Decompiler

The decompiler is written using Python 3, and requires `argparse` and `pprint` packages.

```bash
# set up Python virtual environment
cd dgCHIP8
python3 -m venv ./env
source env/bin/activate

# install required Python packages
(env) pip install -r tools/requirements.txt

# run decompiler
(env) python tools/decompiler.py games/TETRIS
0x0200    a2b4    ■_■___■_ ■_■■_■__    LD I, 2b4             // Set value of register I to 2b4
0x0202    23e6    __■___■■ ■■■__■■_    CALL 3e6              // Call subroutine at 0x3e6
0x0204    22b6    __■___■_ ■_■■_■■_    CALL 2b6              // Call subroutine at 0x2b6
0x0206    7001    _■■■____ _______■    ADD V0, 01            // Add 01 to V0
0x0208    d011    ■■_■____ ___■___■    DRW V0, V1, 1         // Display 1-byte sprite starting at memory location I at (V0, V1), Vf = collision
0x020a    3025    __■■____ __■__■_■    SE V0, 25             // Skip next instruction if V0 == 25
. . .
```

## Resources

* [CHIP-8 Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)

* [How To Write a Computer Emulator](http://fms.komkon.org/EMUL8/HOWTO.html)

* [Wikipedia article on CHIP-8](https://en.wikipedia.org/wiki/CHIP-8)
  
* [(Super)CHIP 8 Secrets](https://github.com/AfBu/haxe-chip-8-emulator/wiki/(Super)CHIP-8-Secrets)
