#include "SDL_FontCache.cpp"
#include <iostream>
#include <vector>
#include <array>
#include "print.cpp"
#include "SDL.h"
#include <stdio.h>
#include <ctime>
#include <string>
#include "globals.cpp"
#include "line.cpp"
#include "draw.cpp"

int main(int argc, char *argv[])
{
    if(SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        print("SDL2 failed");
        return -1;
    }
    
    TTF_Init();
    bool quit = false;
    SDL_Event e;
    SDL_Window *window =  SDL_CreateWindow("Ed", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,
                                           1024, 768, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);;
    
    //font.name = FC_CreateFont();
    //FC_LoadFont(font.name, renderer, "liberation-mono.ttf", 16, FC_MakeColor(143, 175, 127, 255), TTF_STYLE_NORMAL);
    font.name = TTF_OpenFont("liberation-mono.ttf", 16);
    
    
    // Init dummy heads
    headA->prev = NULL;
    headA->next = NULL;
    headB->prev = NULL;
    headB->next = NULL;
    
    bufferA.head = headA;
    bufferB.head = headB;
    
    //font.height = FC_GetHeight(font.name, "A");
    //font.width = FC_GetWidth(font.name, "A");
    font.height = TTF_FontHeight(font.name);
    TTF_SizeText(font.name, "A", &font.width, 0);
    
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
    
    //ioeroereoiru
    
    node *a = InsertLineAt(&bufferA, 0);
    
    //TEST
    SDL_Color textColor = {143, 175, 127, 255};
    SDL_Color bgc = {21, 12, 42, 255};
    SDL_Surface *message = TTF_RenderText_Shaded(font.name, "SDL_Surface", textColor, bgc);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, message);
    SDL_Rect rect = {4,24,font.width,font.height};
    
    
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
                for(int i=0; i<SDL_TEXTINPUTEVENT_TEXT_SIZE; ++i)
                {
                    char c = e.text.text[i];
                    // cancel if a non-ascii char is encountered
                    if(c < ' ' || c > '~')
                    {
                        break;
                    }
                    
                    //strcat(a->data, e.text.text);
                    //U8_strinsert(a->data, bufferA.cursor.column, e.text.text, 1024);
                    memset(a->data+bufferA.cursor.column, e.text.text[0], 1);
                    //print(a->data+bufferA.cursor.column);
                    bufferA.cursor.column++;
                }	
            }
            
            if(e.type == SDL_WINDOWEVENT)
            {
                if(e.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    SDL_GetWindowSize(window, &ww, &wh);
                    bufferA.panel.x = 1;
                    bufferA.panel.y = 1;
                    bufferA.panel.w = (ww / 2) - 1;
                    bufferA.panel.h = wh - 1;
                    
                    bufferB.panel.x = (ww / 2) + 1;
                    bufferB.panel.y = 1;
                    bufferB.panel.w = (ww / 2) - 1;
                    bufferB.panel.h = wh - 1;
                }
            }
            
            if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDLK_ESCAPE)
                {
                    quit = true;
                }
                if(e.key.keysym.sym == SDLK_BACKSPACE)
                {
                    if(bufferA.cursor.column > 0)
                    {
                        bufferA.cursor.column--;
                        U8_strdel(a->data, bufferA.cursor.column);
                    }
                    else
                    {
                        if(a->prev != headA)
                        {
                            DeleteLineAt(&bufferA, bufferA.cursor.line);
                            a = a->prev;
                            
                            bufferA.cursor.line--;
                            bufferA.cursor.column = strlen(a->data);
                            //bufferA.line_count--;
                        }
                    }
                }
                if(e.key.keysym.sym == SDLK_LEFT)
                {
                    if(bufferA.cursor.column > 0)
                    {
                        bufferA.cursor.column--;
                    }
                    else
                    {
                        if(a->prev != headA)
                        {
                            a = a->prev;
                            bufferA.cursor.line--;
                            bufferA.cursor.column = strlen(a->data);
                        }
                    }
                }
                if(e.key.keysym.sym == SDLK_RIGHT)
                {
                    if(bufferA.cursor.column < strlen(a->data))
                    {
                        bufferA.cursor.column++;
                    }
                    else
                    {
                        if(a->next != NULL)
                        {
                            a = a->next;
                            bufferA.cursor.line++;
                            bufferA.cursor.column = 0;
                        }
                    }
                }
                if(e.key.keysym.sym == SDLK_RETURN)
                {
                    U8_strinsert(a->data, bufferA.cursor.column+1, "\n", 1024);
                    bufferA.cursor.column = 0;
                    bufferA.cursor.line++;
                    a = InsertLineAt(&bufferA, bufferA.line_count);
                }
                
                if(e.key.keysym.sym == SDLK_TAB)
                {
                    U8_strinsert(a->data, bufferA.cursor.column, "    ", 1024);
                    bufferA.cursor.column += 4;
                }
                if (e.key.keysym.sym == SDLK_a)
                {
                    //a->data[0] = (char)'a';
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
        
        SDL_SetRenderDrawColor(renderer, 21, 12, 42, 255);// background
        
        SDL_Rect t = {4*font.width,0,font.width,font.height};
        SDL_RenderCopy(renderer, texture, &t, &rect);
        
        char ch[1];
        node *cc= headA;
        for (int i = 0; i < bufferA.line_count; ++i)
        {
            cc = cc->next;
            
            
            for (int j = 0; j < strlen(cc->data); ++j)
            {
                //char *ch = &cc->data[j];
                //std::string ch = &cc->data[j];
                //ch = ch.at(0);
                //strncpy(ch, cc->data, 1);
                
                //FC_Draw(font.name, renderer, (j * font.width) + margin, (i * font.height) + margin, ch);
            }
            
            //FC_Draw(font.name, renderer, 4, (i * font.height) + margin, cc->data);
        }
        
        //FC_Draw(font.name, renderer, 600, 100, std::to_string(bufferA.cursor.line+1).c_str());
        //FC_Draw(font.name, renderer, 600, 140, std::to_string(bufferA.cursor.column+1).c_str());
        
        SDL_RenderPresent(renderer);
    }
    
    //FC_FreeFont(font.name);
    SDL_FreeSurface(message);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}