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
    
    
    bool quit = false;
    SDL_Event e;
    SDL_Window *window =  SDL_CreateWindow("Ed", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,
                                           1024, 768, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);;
    
    font.name = FC_CreateFont();
    FC_LoadFont(font.name, renderer, "liberation-mono.ttf", 16, FC_MakeColor(0,255,0,255), TTF_STYLE_NORMAL);
    
    
    
    // Init dummy head
    headA->prev = NULL;
    headB->next = NULL;
    
    bufferA.head = headA;
    bufferB.head = headB;
    
    font.height = FC_GetHeight(font.name, "A");
    font.width = FC_GetWidth(font.name, "A");
    
    int ww;
    int wh;
    
    SDL_GetWindowSize(window, &ww, &wh);
    bufferA.panel.x = 1;
    bufferA.panel.y = 1;
    bufferA.panel.w = (ww / 2) - 1;
    bufferA.panel.h = wh - 1;
    
    bufferB.panel.x = (ww / 2) + 1;
    bufferB.panel.y = 1;
    bufferB.panel.w = (ww / 2) - 1;
    bufferB.panel.h = wh - 1;
    
    
    //char *test1 = "Hello";
    //char *test2 = "World";
    //char *test3 = "My";
    //char *test4 = "Test";
    //
    //node *a = InsertLineAt(&bufferL, 0);
    //node *b = InsertLineAt(&bufferL, 1);
    //node *c = InsertLineAt(&bufferL, 2);
    //node *d = InsertLineAt(&bufferL, 3);
    //
    //for (int i = 0; i < 5; ++i)
    //{
    //a->data[i] = test1[i];
    //b->data[i] = test2[i];
    //c->data[i] = test3[i];
    //d->data[i] = test4[i];
    //}
    //
    //node *test = headL->next;
    //while(test != NULL)
    //{
    //print(test->data);
    //test = test->next;
    //}
    
    
    
    node *a = InsertLineAt(&bufferA, 0);
    
    while(!quit)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
            
            if(e.type == SDL_TEXTINPUT)
            {
                strcat(a->data, e.text.text);
                bufferA.cursor.column++;
            }
            
            if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDLK_ESCAPE)
                {
                    quit = true;
                }
                if(e.key.keysym.sym == SDLK_BACKSPACE)
                {
                    bufferA.cursor.column--;
                    U8_strdel(a->data, bufferA.cursor.column);
                }
                if(e.key.keysym.sym == SDLK_RETURN)
                {
                    bufferA.cursor.column = 0;
                    bufferA.cursor.line++;
                    //U8_strinsert(a->data, cursorL.col, "\n", 1024);
                    a = InsertLineAt(&bufferA, bufferA.line_count);
                }
                if (e.key.keysym.sym == SDLK_a)
                {
                    //a->data[0] = (char)'a';
                    print(a->data);
                }
                if (e.key.keysym.sym == SDLK_z)
                {
                    //a->data[0] = (char)'z';
                }
            }
        }
        
        SDL_RenderClear(renderer);
        
        PanelDraw(renderer, bufferA);
        PanelDraw(renderer, bufferB);
        
        CursorDraw(renderer, bufferA);
        CursorDraw(renderer, bufferB);
        
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);// background
        
        node *cc= headA;
        for (int i = 0; i <= bufferA.cursor.line; ++i)
        {
            cc = cc->next;
            FC_Draw(font.name, renderer, 4, i * font.height, cc->data);
        }
        
        SDL_RenderPresent(renderer);
    }
    
    
    FC_FreeFont(font.name);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}