#include "SDL_FontCache.cpp"
#include <iostream>
#include <array>
#include "print.cpp"
#include "SDL.h"
//#include <SDL_ttf.h>
#include <stdio.h>
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
    
    char all_ascii[95];
    for (int i = 0; i <= 95; ++i)
    {
        all_ascii[i] = 32 + i;
    }
    
    TTF_Init();
    bool quit = false;
    SDL_Event e;
    SDL_Window *window =  SDL_CreateWindow("Ed", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,
                                           1024, 768, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);;
    
    font.name = TTF_OpenFont("liberation-mono.ttf", 16);
    
    
    // Init dummy heads
    headA->prev = NULL;
    headA->next = NULL;
    headB->prev = NULL;
    headB->next = NULL;
    
    bufferA.head = headA;
    //bufferB.head = headB;
    
    
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
    
    node *a = InsertLineAt(&bufferA, 0);
    memset(a->data, 0, 128);
    print (a->data);
    
    //TEST
    SDL_Color textColor = {143, 175, 127, 255};
    SDL_Color bgc = {21, 12, 42, 255};
    SDL_Surface *characters_surface = TTF_RenderText_Blended(font.name, all_ascii, textColor);
    SDL_Texture *characters_texture = SDL_CreateTextureFromSurface(renderer, characters_surface);
    SDL_Surface *screen_surf = SDL_GetWindowSurface(window);
    SDL_Texture *screen_texture = SDL_CreateTextureFromSurface(renderer, screen_surf);
    
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
                //for(int i=0; i<SDL_TEXTINPUTEVENT_TEXT_SIZE; ++i)
                //{
                //char c = e.text.text[i];
                //cancel if a non-ascii char is encountered
                //if(c < ' ' || c > '~')
                //{
                //break;
                //}
                //
                //
                //}	
                //strcat(a->data, e.text.text);
                U8_strinsert(a->data, bufferA.cursor.column, e.text.text, 1024);
                memset(a->data+bufferA.cursor.column, e.text.text[0], 1);
                //char v;
                //strcpy(&v, &a->data[0]+bufferA.cursor.column);
                //print((int)v);
                bufferA.cursor.column++;
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
                    memset(a, 0, 128);
                }
                
                if(e.key.keysym.sym == SDLK_TAB)
                {
                    //U8_strinsert(a->data, bufferA.cursor.column, "    ", 1024);
                    bufferA.cursor.column += 4;
                }
            }
        }
        
        SDL_RenderClear(renderer);
        
        PanelDraw(renderer, bufferA);
        PanelDraw(renderer, bufferB);
        
        CursorDraw(renderer, bufferA);
        CursorDraw(renderer, bufferB);
        
        SDL_SetRenderDrawColor(renderer, 21, 12, 42, 255);// background
        
        //SDL_Rect t = {0,0,font.width,font.height};
        //SDL_RenderCopy(renderer, texture, &t, &rect);
        
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
                //print(cc->data[j]);
                int cur_char_code = (int)cc->data[j];
                //print(cur_char_code);
                SDL_Rect glyph_rect = {(cur_char_code - 32)*font.width,0,font.width,font.height};
                SDL_Rect t = {4+(j*font.width),4+(i*font.height),font.width,font.height};
                SDL_RenderCopy(renderer, characters_texture, &glyph_rect, &t);
            }
        }
        
        SDL_RenderPresent(renderer);
    }
    
    SDL_FreeSurface(characters_surface);
    SDL_FreeSurface(screen_surf);
    SDL_DestroyTexture(characters_texture);
    SDL_DestroyTexture(screen_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}