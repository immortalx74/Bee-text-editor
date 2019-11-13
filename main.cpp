#include "SDL_FontCache.cpp"
#include <iostream>
#include <vector>
#include <array>
#include "print.cpp"
#include "SDL.h"
#include <stdio.h>
#include <ctime>
#include "line.cpp"
#include "cursor.cpp"

int main(int argc, char *argv[])
{
    if(SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        print("SDL2 failed");
        return -1;
    }
    
    // Init dummy head
    head->prev = NULL;
    head->next = NULL;
    
    bool quit = false;
    SDL_Event e;
    SDL_Window *window =  SDL_CreateWindow("Ed", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,
                                           1024, 768, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);;
    
    
    FC_Font *font = FC_CreateFont();
    FC_LoadFont(font, renderer, "liberation-mono.ttf", 13, FC_MakeColor(0,255,0,255), TTF_STYLE_NORMAL);
    
    
    char *test1 = "Hello";
    char *test2 = "World";
    char *test3 = "My";
    char *test4 = "Test";
    
    node *a = InsertLineAt(0);
    node *b = InsertLineAt(1);
    node *c = InsertLineAt(2);
    node *d = InsertLineAt(3);
    
    for (int i = 0; i < 5; ++i)
    {
        a->data[i] = test1[i];
        b->data[i] = test2[i];
        c->data[i] = test3[i];
        d->data[i] = test4[i];
    }
    
    node *test = head->next;
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
            }
        }
        
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderClear(renderer);
        
        //FC_Draw(font, renderer, 0, 0, a->data);
        
        SDL_RenderPresent(renderer);
    }
    
    
    FC_FreeFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}