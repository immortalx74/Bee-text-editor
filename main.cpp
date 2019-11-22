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
#include "input.h"

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
                bufferA.cursor.last_hor_pos = bufferA.cursor.column;
            }
            else if (app.e.type == SDL_KEYDOWN)
            {
                if (app.e.key.keysym.sym == SDLK_ESCAPE)
                {
                    app.quit = true;
                }
                else if(app.e.key.keysym.sym == SDLK_RETURN)
                {
                    a = InputReturn(&bufferA, a);
                }
                else if(app.e.key.keysym.sym == SDLK_BACKSPACE)
                {
                    a = InputBackspace(&bufferA, a);
                }
                else if(app.e.key.keysym.sym == SDLK_DELETE)
                {
                    a = InputDelete(&bufferA, a);
                }
                else if(app.e.key.keysym.sym == SDLK_LEFT)
                {
                    a = InputLeft(&bufferA, a);
                }
                else if(app.e.key.keysym.sym == SDLK_RIGHT)
                {
                    a = InputRight(&bufferA, a);
                }
                else if(app.e.key.keysym.sym == SDLK_UP)
                {
                    a = InputUp(&bufferA, a);
                }
                else if(app.e.key.keysym.sym == SDLK_DOWN)
                {
                    a = InputDown(&bufferA, a);
                }
                else if(app.e.key.keysym.sym == SDLK_TAB)
                {
                    InputTab(&bufferA, a);
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
