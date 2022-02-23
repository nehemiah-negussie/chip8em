#include <stdio.h>
#include <stdlib.h>
#include "instruct.h"
#include <time.h>

#define VIDEO_WIDTH 64
#define VIDEO_HEIGHT 32

int main(){
    int videoScale = 10;
    int cycleDelay = 1;
    char romFilename[] = "BC_test.ch8";
    chip8 a;
    initalizeChip(&a);
    loadROM(&a, romFilename);
    SDL_Init(SDL_INIT_VIDEO);
    
    SDL_Window * window;
    SDL_Renderer * render;
    SDL_Texture * texture;
    
    int output_width = VIDEO_WIDTH * videoScale;
    int output_height = VIDEO_HEIGHT * videoScale;

    SDL_CreateWindowAndRenderer(output_width, output_height, SDL_WINDOW_SHOWN, &window, &render);

    SDL_CreateTexture(render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,VIDEO_WIDTH, VIDEO_HEIGHT);
    
    time_t lastCycleTime = time(NULL) * 1000;

    while (1){
        if (Input(&a) == -1){
            break;
        }
        
        time_t currentTime = time(NULL) * 1000;
        time_t dt = currentTime - lastCycleTime;
        
        if (dt > cycleDelay){
             lastCycleTime = currentTime;
            Cycle(&a);
            Update(texture, render, a.video, sizeof(a.video[0]) * VIDEO_WIDTH);
        }
    }
    

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}