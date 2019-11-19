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
    
    char all_ascii[96];
    for (int i = 0; i <= 95; ++i)
    {
        all_ascii[i] = 32 + i;
    }
    all_ascii[95] = '\0';
    
    TTF_Init();
    bool quit = false;
    SDL_Event e;
    SDL_Window *window =  SDL_CreateWindow("Ed", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,
                                           1024, 768, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);;
    
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
    //SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_MOD);
    
    SDL_Color textColor = {143, 175, 127, 255};
    SDL_Color bgc = {21, 12, 42, 255};
    SDL_Surface *characters_surface = TTF_RenderText_Blended(font.name, all_ascii, textColor);
    SDL_Texture *characters_texture = SDL_CreateTextureFromSurface(renderer, characters_surface);
    SDL_Surface *screen_surface = SDL_GetWindowSurface(window);
    SDL_Texture *screen_texture = SDL_CreateTextureFromSurface(renderer, screen_surface);
    
    //NOTE:set position and size to that of panel
    //NOTE:also update im texture when editing text
    SDL_Texture *im_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, ww/2, wh);
    
    SDL_SetTextureBlendMode(characters_texture, SDL_BLENDMODE_NONE);
    SDL_SetTextureBlendMode(im_texture, SDL_BLENDMODE_BLEND);
    
    
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
                
                
                SDL_SetRenderTarget(renderer, im_texture);
                int cur_char_code = (int)e.text.text[0];
                SDL_Rect glyph_rect = {(cur_char_code - 32)*font.width,0,font.width,font.height};
                SDL_Rect pos = {4+(bufferA.cursor.column*font.width),4+(bufferA.cursor.line*font.height),font.width,font.height};
                SDL_RenderCopy(renderer, characters_texture, &glyph_rect, &pos);
                SDL_SetRenderTarget(renderer, NULL);
                
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
                        
                        SDL_SetRenderTarget(renderer, im_texture);
                        SDL_SetRenderDrawColor(renderer, 21, 12, 42, 0);
                        SDL_Rect char_rect = {bufferA.cursor.column, bufferA.cursor.line, font.width,font.height};
                        SDL_RenderFillRect(renderer, &char_rect);
                        
                        node *current = headA->next;
                        //SDL_RenderClear(renderer);
                        //for (int i = 0; i < bufferA.line_count; ++i)
                        //{
                        for (int j = 0; j < 128; ++j)
                        {
                            //if(current->data[j] != 0)
                            //{
                            int cur_char_code = (int)a->data[j];
                            //print(cur_char_code);
                            SDL_Rect glyph_rect = {(cur_char_code - 32)*font.width,0,font.width,font.height};
                            SDL_Rect pos = {4+(j*font.width),4+(bufferA.cursor.line*font.height),font.width,font.height};
                            SDL_RenderCopy(renderer, characters_texture, &glyph_rect, &pos);
                            //}
                        }
                        
                        //current = current->next;
                        //}
                        
                        SDL_SetRenderTarget(renderer, NULL);
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
        
        //char ch[1];
        node *cc= headA;
        //for (int i = 0; i < bufferA.line_count; ++i)
        //{
        //cc = cc->next;
        //
        //for (int j = 0; j < strlen(cc->data); ++j)
        //{
        //int cur_char_code = (int)cc->data[j];
        //SDL_Rect glyph_rect = {(cur_char_code - 32)*font.width,0,font.width,font.height};
        //SDL_Rect screen_pos = {4+(j*font.width),4+(i*font.height),font.width,font.height};
        //SDL_RenderCopy(renderer, characters_texture, &glyph_rect, &screen_pos);
        //}
        //}
        
        
        SDL_Rect pan = {bufferA.panel.x,bufferA.panel.y,bufferA.panel.w,bufferA.panel.h};
        SDL_RenderCopy(renderer, im_texture, NULL, &pan);
        
        SDL_RenderPresent(renderer);
    }
    
    SDL_FreeSurface(characters_surface);
    SDL_FreeSurface(screen_surface);
    SDL_DestroyTexture(characters_texture);
    SDL_DestroyTexture(screen_texture);
    SDL_DestroyTexture(im_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}