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
#include <assert.h>

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
    
    //START WITH LEFT BUFFER/PANEL
    app.active_buffer = &bufferA;
    
    // first line
    node *a = InsertLineAt(&bufferB, 0);
    memset(a->data, 0, 128);
    a = InsertLineAt(&bufferA, 0);
    memset(a->data, 0, 128);
    
    SDL_Color textColor = {143, 175, 127, 255};
    SDL_Surface *characters_surface = TTF_RenderText_Blended(font.name, app.ascii_sequence, textColor);
    characters_texture = SDL_CreateTextureFromSurface(app.renderer, characters_surface);
    SDL_Surface *screen_surface = SDL_GetWindowSurface(app.window);
    screen_texture = SDL_CreateTextureFromSurface(app.renderer, screen_surface);
    SDL_SetTextureBlendMode(characters_texture, SDL_BLENDMODE_NONE);
    
    //TEST panel textures=============================================================
    bufferA.panel.texture = SDL_CreateTexture(app.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, app.ww/2, app.wh);
    bufferB.panel.texture = SDL_CreateTexture(app.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, app.ww/2, app.wh);
    
    SDL_SetTextureBlendMode(bufferA.panel.texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(bufferB.panel.texture, SDL_BLENDMODE_BLEND);
    
    //===============================================================================
    
    bufferA.head = headA;
    bufferB.head = headB;
    
    while(!app.quit)
    {
        while (SDL_PollEvent(&app.e))
        {
            if(app.e.type == SDL_TEXTINPUT)
            {
                a = InputText(app.active_buffer, a);
            }
            else if (app.e.type == SDL_KEYDOWN)
            {
                if (app.e.key.keysym.sym == SDLK_ESCAPE)
                {
                    app.quit = true;
                }
                else if(app.e.key.keysym.sym == SDLK_RETURN)
                {
                    a = InputReturn(app.active_buffer, a);
                }
                else if(app.e.key.keysym.sym == SDLK_BACKSPACE)
                {
                    a = InputBackspace(app.active_buffer, a);
                }
                else if(app.e.key.keysym.sym == SDLK_DELETE)
                {
                    a = InputDelete(app.active_buffer, a);
                }
                else if(app.e.key.keysym.sym == SDLK_LEFT)
                {
                    a = InputLeft(app.active_buffer, a);
                }
                else if(app.e.key.keysym.sym == SDLK_RIGHT)
                {
                    a = InputRight(app.active_buffer, a);
                }
                else if(app.e.key.keysym.sym == SDLK_UP)
                {
                    a = InputUp(app.active_buffer, a);
                }
                else if(app.e.key.keysym.sym == SDLK_DOWN)
                {
                    a = InputDown(app.active_buffer, a);
                }
                else if(app.e.key.keysym.sym == SDLK_TAB)
                {
                    InputTab(app.active_buffer, a);
                }
                else if( app.e.key.keysym.sym == SDLK_KP_0 && SDL_GetModState() & KMOD_CTRL)
                {
                    if(app.active_buffer == &bufferA)
                    {
                        app.active_buffer = &bufferB;
                        a = GetLineNode(&bufferB, app.active_buffer->line);
                        
                        RenderClearLine(&bufferB, bufferB.head->next, 0, characters_texture, bufferB.panel.texture);
                        RenderClearLine(&bufferA, bufferA.head->next, 0, characters_texture, bufferA.panel.texture);
                    }
                    else
                    {
                        app.active_buffer = &bufferA;
                        a = GetLineNode(&bufferA, app.active_buffer->line);
                        
                        RenderClearLine(&bufferA, bufferA.head->next, 0, characters_texture, bufferA.panel.texture);
                        RenderClearLine(&bufferB, bufferB.head->next, 0, characters_texture, bufferB.panel.texture);
                    }
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
                    
                    bufferA.panel.texture= SDL_CreateTexture(app.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, app.ww/2, app.wh);
                    bufferB.panel.texture= SDL_CreateTexture(app.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, app.ww/2, app.wh);
                    
                    SDL_SetTextureBlendMode(bufferA.panel.texture, SDL_BLENDMODE_BLEND);
                    SDL_SetTextureBlendMode(bufferB.panel.texture, SDL_BLENDMODE_BLEND);
                    
                    RenderClearLine(&bufferA, bufferA.head->next, 0, characters_texture, bufferA.panel.texture);
                    RenderClearLine(&bufferB, bufferB.head->next, 0, characters_texture, bufferB.panel.texture);
                }
            }
        }
        
        SDL_RenderClear(app.renderer);
        
        PanelDraw(app.renderer, &bufferA);
        PanelDraw(app.renderer, &bufferB);
        
        CursorDraw(app.renderer, &bufferA);
        CursorDraw(app.renderer, &bufferB);
        
        SDL_SetRenderDrawColor(app.renderer, 21, 12, 42, 255);// background
        
        SDL_Rect panA = {bufferA.panel.x,bufferA.panel.y,bufferA.panel.w,bufferA.panel.h};
        SDL_Rect panB = {bufferB.panel.x,bufferB.panel.y,bufferB.panel.w,bufferB.panel.h};
        
        SDL_RenderCopy(app.renderer, bufferA.panel.texture, NULL, &panA);
        SDL_RenderCopy(app.renderer, bufferB.panel.texture, NULL, &panB);
        
        SDL_RenderPresent(app.renderer);
    }
    
    SDL_FreeSurface(characters_surface);
    SDL_FreeSurface(screen_surface);
    SDL_DestroyTexture(characters_texture);
    SDL_DestroyTexture(screen_texture);
    SDL_DestroyTexture(bufferA.panel.texture);
    SDL_DestroyTexture(bufferB.panel.texture);
    TTF_CloseFont(font.name);
    SDL_DestroyRenderer(app.renderer);
    SDL_DestroyWindow(app.window);
    SDL_Quit();
    
    return 0;
}
