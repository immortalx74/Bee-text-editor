#include "SDL_FontCache.cpp"
#include <iostream>
#include <vector>
#include <array>
#include "print.cpp"
#include "SDL.h"
#include <stdio.h>
#include <ctime>
#include "globals.cpp"
#include "line.cpp"

int main(int argc, char *argv[])
{
    if(SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        print("SDL2 failed");
        return -1;
    }
    
    // Init dummy head
    headL->prev = NULL;
    headL->next = NULL;
    
    bool quit = false;
    SDL_Event e;
    SDL_Window *window =  SDL_CreateWindow("Ed", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,
                                           1024, 768, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);;
    
    font.name = FC_CreateFont();
    FC_LoadFont(font.name, renderer, "liberation-mono.ttf", 13, FC_MakeColor(0,255,0,255), TTF_STYLE_NORMAL);
    
    font.height = FC_GetHeight(font.name, "A");
    font.width = FC_GetWidth(font.name, "A");
    
    char *test1 = "Hello";
    char *test2 = "World";
    char *test3 = "My";
    char *test4 = "Test";
    
    node *a = InsertLineAt(&bufferL, 0);
    node *b = InsertLineAt(&bufferL, 1);
    node *c = InsertLineAt(&bufferL, 2);
    node *d = InsertLineAt(&bufferL, 3);
    
    for (int i = 0; i < 5; ++i)
    {
        a->data[i] = test1[i];
        b->data[i] = test2[i];
        c->data[i] = test3[i];
        d->data[i] = test4[i];
    }
    
    node *test = headL->next;
    while(test != NULL)
    {
        print(test->data);
        test = test->next;
    }
    
    
    while(!quit)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
            
            if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDLK_ESCAPE)
                {
                    quit = true;
                }
                
                if (e.key.keysym.sym == SDLK_a)
                {
                    a->data[0] = (char)'a';
                }
                if (e.key.keysym.sym == SDLK_z)
                {
                    a->data[0] = (char)'z';
                }
            }
        }
        
        SDL_RenderClear(renderer);
        
        int ww;
        int wh;
        
        SDL_GetWindowSize(window, &ww, &wh);
        panelL.x = 0;
        panelL.y = 0;
        panelL.w = ww / 2;
        panelL.h = wh;
        
        panelR.x = panelL.w;
        panelR.y = 0;
        panelR.w = ww / 2;
        panelR.h = wh;
        
        SDL_Rect panel_boxL = {panelL.x, panelL.y, panelL.w, panelL.h};
        SDL_Rect panel_boxR = {panelR.x, panelR.y, panelR.w, panelR.h};
        SDL_SetRenderDrawColor(renderer, 250, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &panel_boxL);
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 250, 255);
        SDL_RenderDrawRect(renderer, &panel_boxR);
        
        SDL_Rect cur = {4, 0+panelL.margin, font.width, font.height};
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &cur);
        
        
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);// background
        FC_Draw(font.name, renderer, 0 + panelL.margin, 0 + panelL.margin, a->data);
        
        SDL_RenderPresent(renderer);
    }
    
    
    FC_FreeFont(font.name);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}