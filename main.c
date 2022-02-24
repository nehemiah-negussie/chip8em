#include <stdio.h>
#include <stdlib.h>
#include "instruct.h"
#include <sys/time.h>

#define VIDEO_WIDTH 64
#define VIDEO_HEIGHT 32



int main(){
    // Scale up from 64x32 resolution
    int videoScale = 10;
    // Time in ms between cycles
    int cycleDelay = 3;

    char romFilename[] = "Pong.ch8";

    chip8 a;
    initalizeChip(&a);
    loadROM(&a, romFilename);


    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window * window;
    SDL_Renderer * render;
    SDL_Texture * texture;
    
    int output_width = VIDEO_WIDTH * videoScale;
    int output_height = VIDEO_HEIGHT * videoScale;

    window = SDL_CreateWindow("Emulator", 0, 0, output_width, output_height, SDL_WINDOW_SHOWN);

    render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    texture = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, VIDEO_WIDTH, VIDEO_HEIGHT);
    
    // Timer for cycle length
    double deltaTime;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    long long currentTime = tv.tv_sec*1000LL + tv.tv_usec/1000;
    long long lastTime = tv.tv_sec*1000LL + tv.tv_usec/1000;

    while (1){
        // Escape or quit is pressed
        if (Input(&a) == -1){
            break;
        }

        gettimeofday(&tv, NULL);
        currentTime = tv.tv_sec*1000LL + tv.tv_usec/1000;
        deltaTime = currentTime - lastTime;
        
        if (deltaTime > cycleDelay){
            lastTime = currentTime;
            Cycle(&a);
            // Update display output
            Update(texture, render, a.video, sizeof(a.video[0]) * VIDEO_WIDTH);
        }
       
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}