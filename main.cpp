//#include "SDL_FontCache.cpp"
//#include <iostream>
//#include <array>
//#include "SDL.h"
//#include <stdio.h>
//#include <string>
#include "globals.h"
#include "line.h"
#include "draw.h"
#include "character.h"
#include "print.h"

void PrintData(node *head_node)
{
    print("--------------------------------------------");
    
    node *start = head_node->next;
    while(start != NULL)
    {
        print(start->data);
        start = start->next;
    }
    
    print("--------------------------------------------");
    return;
};

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
    SDL_Surface *characters_surface = TTF_RenderText_Blended(font.name, app.ascii_sequence, textColor);
    characters_texture = SDL_CreateTextureFromSurface(app.renderer, characters_surface);
    SDL_Surface *screen_surface = SDL_GetWindowSurface(app.window);
    screen_texture = SDL_CreateTextureFromSurface(app.renderer, screen_surface);
    im_texture = SDL_CreateTexture(app.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, app.ww/2, app.wh);
    
    SDL_SetTextureBlendMode(characters_texture, SDL_BLENDMODE_NONE);
    SDL_SetTextureBlendMode(im_texture, SDL_BLENDMODE_BLEND);
    bufferA.head = headA;
    
    while(!app.quit)
    {
        while (SDL_PollEvent(&app.e))
        {
            if(app.e.type == SDL_TEXTINPUT)
            {
                InsertCharacterAt(&bufferA, a, bufferA.cursor.column);
                RenderCharacterAt(a, bufferA.cursor.line, bufferA.cursor.column - 1, strlen(a->data), characters_texture, im_texture);
            }
            else if (app.e.type == SDL_KEYDOWN)
            {
                if (app.e.key.keysym.sym == SDLK_ESCAPE)
                {
                    app.quit = true;
                }
                else if(app.e.key.keysym.sym == SDLK_BACKSPACE)
                {
                    a = DeleteCharacterAt(&bufferA, a, bufferA.cursor.column);
                    RenderClearCharacterAt(a, bufferA.cursor.line, bufferA.cursor.column, strlen(a->data),characters_texture, im_texture);
                }
                else if(app.e.key.keysym.sym == SDLK_DELETE)
                {
                    if(bufferA.cursor.column < strlen(a->data))
                    {
                        bufferA.cursor.column++;
                        a = DeleteCharacterAt(&bufferA, a, bufferA.cursor.column);
                        RenderClearCharacterAt(a, bufferA.cursor.line, bufferA.cursor.column, strlen(a->data),characters_texture, im_texture);
                    }
                    else if(bufferA.cursor.line < bufferA.line_count - 1)
                    {
                        bufferA.cursor.line++;
                        bufferA.cursor.column = 0;
                        a = a->next;
                        a = DeleteCharacterAt(&bufferA, a, bufferA.cursor.column);
                        RenderClearCharacterAt(a, bufferA.cursor.line, bufferA.cursor.column, strlen(a->data),characters_texture, im_texture);
                    }
                }
                else if(app.e.key.keysym.sym == SDLK_LEFT)
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
                else if(app.e.key.keysym.sym == SDLK_RIGHT)
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
                else if(app.e.key.keysym.sym == SDLK_UP)
                {
                    if(bufferA.cursor.line > 0)
                    {
                        if(bufferA.cursor.column <= strlen(a->prev->data))
                        {
                            bufferA.cursor.line--;
                            a = a->prev;
                        }
                        else
                        {
                            bufferA.cursor.line--;
                            bufferA.cursor.column = strlen(a->prev->data);
                            a = a->prev;
                        }
                    }
                }
                else if(app.e.key.keysym.sym == SDLK_DOWN)
                {
                    if(bufferA.cursor.line < bufferA.line_count - 1)
                    {
                        if(bufferA.cursor.column <= strlen(a->next->data))
                        {
                            bufferA.cursor.line++;
                            a = a->next;
                        }
                        else
                        {
                            bufferA.cursor.line++;
                            bufferA.cursor.column = strlen(a->next->data);
                            a = a->next;
                        }
                    }
                }
                else if(app.e.key.keysym.sym == SDLK_RETURN)
                {
                    if (bufferA.cursor.column == strlen(a->data))//cursor at end of line
                    {
                        bufferA.cursor.column = 0;
                        bufferA.cursor.line++;
                        a = InsertLineAt(&bufferA, bufferA.cursor.line);
                        memset(a, 0, 128);
                        
                        // re-draw trailing lines if this isn't the last line entered
                        if (bufferA.cursor.line <  bufferA.line_count - 1)
                        {
                            RenderClearLine(&bufferA, a, bufferA.cursor.line, characters_texture, im_texture);
                        }
                    }
                    else// cursor at start or middle of line
                    {
                        a = InsertLineAt(&bufferA, bufferA.cursor.line+1);
                        memset(a, 0, 128);
                        
                        int index = 0;
                        int len = strlen(a->prev->data);
                        
                        for (int i = bufferA.cursor.column; i < len; ++i)
                        {
                            a->data[index] = a->prev->data[i];
                            a->prev->data[i] = '\0';
                            index++;
                        }
                        
                        RenderClearLine(&bufferA, a->prev, bufferA.cursor.line, characters_texture, im_texture);
                        
                        bufferA.cursor.line++;
                        bufferA.cursor.column = 0;
                    }
                }
                
                else if(app.e.key.keysym.sym == SDLK_TAB)
                {
                    //U8_strinsert(a->data, bufferA.cursor.column, "    ", 1024);
                    //bufferA.cursor.column += 4;
                }
            }
            else if (app.e.type == SDL_QUIT)
            {
                app.quit = true;
            }
            else if(app.e.type == SDL_WINDOWEVENT)
            {
                if(app.e.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    WindowResize(&app, app.window);
                    SDL_DestroyTexture(im_texture);
                    im_texture = SDL_CreateTexture(app.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, app.ww/2, app.wh);
                    SDL_SetTextureBlendMode(im_texture, SDL_BLENDMODE_BLEND);
                    RenderClearLine(&bufferA, headA->next, 0, characters_texture, im_texture);
                    PrintData(headA);
                    
                }
            }
        }
        
        SDL_RenderClear(app.renderer);
        
        PanelDraw(app.renderer, bufferA);
        PanelDraw(app.renderer, bufferB);
        
        CursorDraw(app.renderer, bufferA);
        CursorDraw(app.renderer, bufferB);
        
        SDL_SetRenderDrawColor(app.renderer, 21, 12, 42, 255);// background
        
        SDL_Rect pan = {bufferA.panel.x,bufferA.panel.y,bufferA.panel.w,bufferA.panel.h};
        
        SDL_RenderCopy(app.renderer, im_texture, NULL, &pan);
        
        SDL_RenderPresent(app.renderer);
    }
    
    SDL_FreeSurface(characters_surface);
    SDL_FreeSurface(screen_surface);
    SDL_DestroyTexture(characters_texture);
    SDL_DestroyTexture(screen_texture);
    SDL_DestroyTexture(im_texture);
    TTF_CloseFont(font.name);
    SDL_DestroyRenderer(app.renderer);
    SDL_DestroyWindow(app.window);
    SDL_Quit();
    
    return 0;
}