#include <iostream>
#include <fstream>
//#include <array>
#include "SDL.h"
//#include <stdio.h>
//#include <string>
#include "globals.h"
#include "line.h"
#include "draw.h"
#include "character.h"
#include "print.h"
#include "input.h"
#include "file.h"
#include "list.h"
#include "tinydir.h"

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
    app.last_path = XstringCreate(SDL_GetBasePath());
    WindowResize(&app, app.window);
    
    //====================
    //app.custom_mode = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_DST_ALPHA,
    //SDL_BLENDFACTOR_ONE,
    //SDL_BLENDOPERATION_ADD,
    //SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
    //SDL_BLENDFACTOR_ONE,
    //SDL_BLENDOPERATION_ADD);
    
#if 0    
    // results in black cursor with the green text
    app.custom_mode = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_DST_COLOR,
                                                 SDL_BLENDFACTOR_SRC_ALPHA,
                                                 SDL_BLENDOPERATION_ADD,
                                                 SDL_BLENDFACTOR_ZERO,
                                                 SDL_BLENDFACTOR_ONE,
                                                 SDL_BLENDOPERATION_ADD);
#endif
    // this is the one we prob want
    // yyayyyyyyyyyyyyyyyyy <3 <3 :D
    app.custom_mode = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ONE_MINUS_DST_COLOR,
                                                 SDL_BLENDFACTOR_ZERO,
                                                 SDL_BLENDOPERATION_ADD,
                                                 SDL_BLENDFACTOR_ONE,
                                                 SDL_BLENDFACTOR_ONE,
                                                 SDL_BLENDOPERATION_ADD);
    
    
    //START WITH LEFT BUFFER
    app.active_buffer = &bufferA;
    
    // Add a line to both buffers
    InsertLineAt(&bufferA, 0);
    InsertLineAt(&bufferB, 0);
    
    // Set textures/surfaces
    
    SDL_Surface *characters_surface = TTF_RenderText_Blended(font.name, app.ascii_sequence, font.text_color);
    characters_texture = SDL_CreateTextureFromSurface(app.renderer, characters_surface);
    
    SDL_Surface *bar_characters_surface = TTF_RenderText_Blended(font.name, app.ascii_sequence, bufferA.status_bar.text_color);
    bar_characters_texture = SDL_CreateTextureFromSurface(app.renderer, bar_characters_surface);
    
    
    SDL_Surface *screen_surface = SDL_GetWindowSurface(app.window);
    screen_texture = SDL_CreateTextureFromSurface(app.renderer, screen_surface);
    SDL_SetTextureBlendMode(characters_texture, SDL_BLENDMODE_NONE);
    
    bufferA.panel.texture = SDL_CreateTexture(app.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, bufferA.panel.w, bufferA.panel.h);
    bufferB.panel.texture = SDL_CreateTexture(app.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, bufferB.panel.w, bufferB.panel.h);
    
    SDL_SetTextureBlendMode(bufferA.panel.texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(bufferB.panel.texture, SDL_BLENDMODE_BLEND);
    
    
    bufferA.status_bar.texture = SDL_CreateTexture(app.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, bufferA.status_bar.w, bufferA.status_bar.h);
    bufferB.status_bar.texture = SDL_CreateTexture(app.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, bufferB.status_bar.w, bufferB.status_bar.h);
    SDL_SetTextureBlendMode(bufferA.status_bar.texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(bufferB.status_bar.texture, SDL_BLENDMODE_BLEND);
    
    
    while(!app.quit)
    {
        while (SDL_PollEvent(&app.e))
        {
            // INPUT EVENTS
            if(app.mode == TEXT_EDIT)
            {
                GetTextEditingInput();
                GetBindedCommandsInput();
            }
            else if (app.mode == LIST_NAV)
            {
                GetListNavigationInput();
            }
            
            GetGlobalInput();
            
            // OTHER EVENTS
            if (app.e.type == SDL_QUIT)
            {
                app.quit = true;
            }
            else if(app.e.type == SDL_WINDOWEVENT)
            {
                if(app.e.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    WindowResize(&app, app.window);
                    
                    SDL_DestroyTexture(bufferA.panel.texture);
                    SDL_DestroyTexture(bufferB.panel.texture);
                    bufferA.panel.texture= SDL_CreateTexture(app.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, bufferA.panel.w, bufferA.panel.h);
                    bufferB.panel.texture= SDL_CreateTexture(app.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, bufferB.panel.w, bufferB.panel.h);
                    
                    SDL_SetTextureBlendMode(bufferA.panel.texture, SDL_BLENDMODE_BLEND);
                    SDL_SetTextureBlendMode(bufferB.panel.texture, SDL_BLENDMODE_BLEND);
                    
                    RenderClearLine(&bufferA, 0, characters_texture, bufferA.panel.texture);
                    RenderClearLine(&bufferB, 0, characters_texture, bufferB.panel.texture);
                    
                    SDL_DestroyTexture(bufferA.status_bar.texture);
                    SDL_DestroyTexture(bufferB.status_bar.texture);
                    
                    bufferA.status_bar.texture = SDL_CreateTexture(app.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, bufferA.status_bar.w, bufferA.status_bar.h);
                    bufferB.status_bar.texture = SDL_CreateTexture(app.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, bufferB.status_bar.w, bufferB.status_bar.h);
                    SDL_SetTextureBlendMode(bufferA.status_bar.texture, SDL_BLENDMODE_BLEND);
                    SDL_SetTextureBlendMode(bufferB.status_bar.texture, SDL_BLENDMODE_BLEND);
                }
            }
        }
        
        SDL_RenderClear(app.renderer);
        
        PanelDraw(&bufferA);
        PanelDraw(&bufferB);
        
        BarDraw(&bufferA);
        BarDraw(&bufferB);
        
        
        if(app.mode == TEXT_EDIT)
        {
            //HighlightLineDraw(app.active_buffer);
            //CursorDraw(&bufferA);
            CursorDraw(&bufferB);
        }
        if(app.mode == LIST_NAV)
        {
            HighlightListSelectionDraw(app.active_buffer, app.active_buffer->lst);
            RenderListRange(app.active_buffer, app.active_buffer->lst->scroll_offset, app.active_buffer->lst->element_count, characters_texture, app.active_buffer->panel.texture);
        }
        
        SDL_SetRenderDrawColor(app.renderer, 21, 12, 42, 255);// background
        
        SDL_Rect panA = {bufferA.panel.x,bufferA.panel.y,bufferA.panel.w,bufferA.panel.h};
        SDL_Rect panB = {bufferB.panel.x,bufferB.panel.y,bufferB.panel.w,bufferB.panel.h};
        
        SDL_RenderCopy(app.renderer, bufferA.panel.texture, NULL, &panA);
        SDL_RenderCopy(app.renderer, bufferB.panel.texture, NULL, &panB);
        
        //test blend
        CursorDraw(&bufferA);
        
        SDL_Rect barA = {bufferA.status_bar.x,bufferA.status_bar.y,bufferA.status_bar.w,bufferA.status_bar.h};
        SDL_Rect barB = {bufferB.status_bar.x,bufferB.status_bar.y,bufferB.status_bar.w,bufferB.status_bar.h};
        
        SDL_RenderCopy(app.renderer, bufferA.status_bar.texture, NULL, &barA);
        SDL_RenderCopy(app.renderer, bufferB.status_bar.texture, NULL, &barB);
        
        SDL_RenderPresent(app.renderer);
    }
    
    SDL_FreeSurface(characters_surface);
    SDL_FreeSurface(bar_characters_surface);
    SDL_FreeSurface(screen_surface);
    SDL_DestroyTexture(bufferA.status_bar.texture);
    SDL_DestroyTexture(bufferB.status_bar.texture);
    SDL_DestroyTexture(characters_texture);
    SDL_DestroyTexture(bar_characters_texture);
    SDL_DestroyTexture(screen_texture);
    SDL_DestroyTexture(bufferA.panel.texture);
    SDL_DestroyTexture(bufferB.panel.texture);
    TTF_CloseFont(font.name);
    SDL_DestroyRenderer(app.renderer);
    SDL_DestroyWindow(app.window);
    SDL_Quit();
    
    return 0;
}
