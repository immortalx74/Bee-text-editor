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
#include "character.cpp"
#include "draw.cpp"

int main(int argc, char *argv[])
{
    
    app.Init();
    WindowResize(&app, app.window);
    
    font.name = TTF_OpenFont("liberation-mono.ttf", 16);
    font.height = TTF_FontHeight(font.name);
    TTF_SizeText(font.name, "A", &font.width, 0);
    
    // first line
    node *a = InsertLineAt(&bufferA, 0);
    memset(a->data, 0, 128);
    
    
    SDL_Color textColor = {143, 175, 127, 255};
    SDL_Color bgc = {21, 12, 42, 255};
    SDL_Surface *characters_surface = TTF_RenderText_Blended(font.name, app.ascii_sequence, textColor);
    SDL_Texture *characters_texture = SDL_CreateTextureFromSurface(app.renderer, characters_surface);
    SDL_Surface *screen_surface = SDL_GetWindowSurface(app.window);
    SDL_Texture *screen_texture = SDL_CreateTextureFromSurface(app.renderer, screen_surface);
    
    //NOTE:set position and size to that of panel
    //NOTE:also update im texture when editing text
    SDL_Texture *im_texture = SDL_CreateTexture(app.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, app.ww/2, app.wh);
    
    SDL_SetTextureBlendMode(characters_texture, SDL_BLENDMODE_NONE);
    SDL_SetTextureBlendMode(im_texture, SDL_BLENDMODE_BLEND);
    bufferA.head = headA;
    
    while(!app.quit)
    {
        while (SDL_PollEvent(&app.e))
        {
            if (app.e.type == SDL_QUIT)
            {
                app.quit = true;
            }
            
            if(app.e.type == SDL_TEXTINPUT)
            {
                InsertCharacterAt(&bufferA, a, bufferA.cursor.column);
                RenderCharacterAt(bufferA.cursor.line, bufferA.cursor.column, strlen(a->data), characters_texture, im_texture);
                bufferA.cursor.column++;
            }
            
            if(app.e.type == SDL_WINDOWEVENT)
            {
                if(app.e.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    WindowResize(&app, app.window);
                }
            }
            
            if (app.e.type == SDL_KEYDOWN)
            {
                if (app.e.key.keysym.sym == SDLK_ESCAPE)
                {
                    app.quit = true;
                }
                
                //NOTE:if there are trailing characters AFTER the deleted character,
                // and if cursor column is about to jump to previous row,
                // move all trailing characters to the previous line!
                if(app.e.key.keysym.sym == SDLK_BACKSPACE)
                {
                    if(bufferA.cursor.column > 0)
                    {
                        bufferA.cursor.column--;
                        U8_strdel(a->data, bufferA.cursor.column);
                        
                        SDL_SetRenderTarget(app.renderer, im_texture);
                        SDL_SetRenderDrawColor(app.renderer, 21, 12, 42, 0);
                        node *current = headA->next;
                        
                        //NOTE:hardcoded this rect's width for now
                        SDL_Rect char_rect = {4+bufferA.cursor.column*font.width, 4+bufferA.cursor.line*font.height, 150,font.height};
                        SDL_RenderFillRect(app.renderer, &char_rect);
                        
                        for (int j = 0; j < 128; ++j)
                        {
                            
                            
                            int cur_char_code = (int)a->data[j];
                            SDL_Rect glyph_rect = {(cur_char_code - 32)*font.width,0,font.width,font.height};
                            SDL_Rect pos = {4+(j*font.width),4+(bufferA.cursor.line*font.height),font.width,font.height};
                            
                            SDL_RenderCopy(app.renderer, characters_texture, &glyph_rect, &pos);
                            
                        }
                        
                        SDL_SetRenderTarget(app.renderer, NULL);
                    }
                    else
                    {
                        if(a->prev != headA)
                        {
                            DeleteLineAt(&bufferA, bufferA.cursor.line);
                            a = a->prev;
                            bufferA.cursor.line--;
                            bufferA.cursor.column = strlen(a->data);
                        }
                    }
                }
                if(app.e.key.keysym.sym == SDLK_LEFT)
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
                if(app.e.key.keysym.sym == SDLK_RIGHT)
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
                if(app.e.key.keysym.sym == SDLK_RETURN)
                {
                    U8_strinsert(a->data, bufferA.cursor.column+1, "\n", 1024);
                    bufferA.cursor.column = 0;
                    bufferA.cursor.line++;
                    a = InsertLineAt(&bufferA, bufferA.line_count);
                    memset(a, 0, 128);
                }
                
                if(app.e.key.keysym.sym == SDLK_TAB)
                {
                    //U8_strinsert(a->data, bufferA.cursor.column, "    ", 1024);
                    bufferA.cursor.column += 4;
                }
            }
        }
        
        SDL_RenderClear(app.renderer);
        
        PanelDraw(app.renderer, bufferA);
        PanelDraw(app.renderer, bufferB);
        
        CursorDraw(app.renderer, bufferA);
        CursorDraw(app.renderer, bufferB);
        
        SDL_SetRenderDrawColor(app.renderer, 21, 12, 42, 255);// background
        
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
        SDL_RenderCopy(app.renderer, im_texture, NULL, &pan);
        
        SDL_RenderPresent(app.renderer);
    }
    
    SDL_FreeSurface(characters_surface);
    SDL_FreeSurface(screen_surface);
    SDL_DestroyTexture(characters_texture);
    SDL_DestroyTexture(screen_texture);
    SDL_DestroyTexture(im_texture);
    SDL_DestroyRenderer(app.renderer);
    SDL_DestroyWindow(app.window);
    SDL_Quit();
    
    return 0;
}