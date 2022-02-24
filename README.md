# chip8em
This is a CHIP-8 emulator recreated using C.

![Sample image of Pong gameplay](https://raw.githubusercontent.com/nehemiah-negussie/chip8em/main/sample/window_screenshot.png)



### Requirements:
To use this emulator, you need the SDL library to be installed (SDL 2).  The path to the header files must be included in the command line usage, following the `-I` flag. 

### Usage:
~~~bash
gcc main.c ./include/instruct.c -I/*PATH_TO_SDL_HEADER* -I./include/ -F*PATH_TO_FRAMEWORKS* -framework SDL2
~~~

To change the window size, use the `videoScale` variable to give an integer multiplier to the 64x32 base resolution.
To change clock speed, use the `cycleDelay` variable to edit the time in ms between cycles.

`romFilename` is a string with the path to the .ch8 ROM filename.
