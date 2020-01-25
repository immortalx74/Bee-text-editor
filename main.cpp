#include <fstream>
#include "globals.h"
#include "render.h"
#include "clipboard.h"
//#include <windows.h>

int main(int argc, char *argv[])
{
    bufferA.filename = XstringCreate("*no file*");
    bufferB.filename = XstringCreate("*no file*");
    
    SettingsSetDefaults();
    FileParseSettings();
    Init();
    
    clipboard.text = XstringCreate("");
    ClipBoardGetExternal();
    WindowResize(&app, app.window);
    
    SDL_Cursor *cursor_default;
    cursor_default = SDL_GetDefaultCursor();
    SDL_Cursor *cursor_resize_hor;
    cursor_resize_hor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
    
    
    timer.interval = settings.cursor_blink_rate;
    //START WITH LEFT BUFFER
    app.active_buffer = &bufferA;
    
    // Add a line to both buffers
    LineInsert(&bufferA, 0);
    LineInsert(&bufferB, 0);
    
    // Set textures/surfaces
    SDL_Surface *characters_surface = TTF_RenderText_Blended(font.handle, app.ascii_sequence, font.text_color);
    characters_texture = SDL_CreateTextureFromSurface(app.renderer, characters_surface);
    
    SDL_Surface *bar_characters_surface = TTF_RenderText_Blended(font.handle, app.ascii_sequence, bufferA.status_bar.text_color);
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
    
    bool mb_left_pressed = false;
    static bool focus_lost = false;
    int mx , my;
    
    unsigned int lastTime = 0, currentTime = 0;
    timer.reset();
    
    while(!app.quit)
    {
        int flags = SDL_GetWindowFlags(app.window);
        if(flags & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(16);
        }
        
        if(focus_lost)
        {
            if(SDL_WaitEvent(&app.e))
            {
                if(app.e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
                {
                    focus_lost = false;
                }
            }
        }
        
        while(SDL_PollEvent(&app.e))
        {
            // INPUT EVENTS
            if(app.mode == TEXT_EDIT)
            {
                ProcessInput_TextEditing();
                ProcessInput_Commands();
            }
            else if (app.mode == LIST_NAV)
            {
                ProcessInput_ListNavigation();
            }
            
            ProcessInput_Global();
            
            // GET MOUSE
            SDL_GetMouseState(&mx,&my);
            
            if(app.mode == TEXT_EDIT)// Switch buffer with mouse button
            {
                if(mx < bufferA.panel.w - 4 && SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT))
                {
                    if(!mb_left_pressed)
                    {
                        app.active_buffer = &bufferA;
                        CursorSetToMouse(&bufferA, mx, my);
                    }
                }
                else if(mx > bufferB.panel.x + 4 && SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT))
                {
                    if(!mb_left_pressed)
                    {
                        app.active_buffer = &bufferB;
                        CursorSetToMouse(&bufferB, mx, my);
                    }
                }
            }
            
            if(mx > bufferA.panel.w - 4 && mx < bufferB.panel.x + 4)
            {
                if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT))
                {
                    mb_left_pressed = true;
                }
                SDL_SetCursor(cursor_resize_hor);
            }
            else
            {
                SDL_SetCursor(cursor_default);
            }
            
            if(app.e.type == SDL_MOUSEBUTTONUP)
            {
                SDL_SetCursor(cursor_default);
                mb_left_pressed = false;
            }
            
            // OTHER EVENTS
            if (app.e.type == SDL_QUIT)
            {
                app.quit = true;
            }
            else if(app.e.type == SDL_WINDOWEVENT)
            {
                if(app.e.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
                {
                    focus_lost = true;
                }
                else if(app.e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
                {
                    focus_lost = false;
                }
                if(app.e.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    WindowResize(&app, app.window);
                }
            }
            else if(app.e.type == SDL_MOUSEWHEEL)//TEST:MOUSE SCROLLING
            {
                Input_Scroll(app.active_buffer);
            }
            
        }
        
        if(mb_left_pressed)
        {
            PanelsResize(mx, cursor_resize_hor);
        }
        
        SDL_RenderClear(app.renderer);
        
        RenderPanel(&bufferA);
        RenderPanel(&bufferB);
        
        RenderStatusBar(&bufferA);
        RenderStatusBar(&bufferB);
        
        if(settings.draw_line_highlight && app.mode == TEXT_EDIT)
        {
            RenderLineHighlight(app.active_buffer);
        }
        else if(app.mode == LIST_NAV)
        {
            RenderListElementHighlight(app.active_buffer, app.active_buffer->lst);
            RenderListRange(app.active_buffer, app.active_buffer->lst->scroll_offset, app.active_buffer->lst->element_count, characters_texture, app.active_buffer->panel.texture);
        }
        
        SDL_SetRenderDrawColor(app.renderer, settings.color_background.r, settings.color_background.g, settings.color_background.b, settings.color_background.a);// background
        
        SDL_Rect panA = {bufferA.panel.x,bufferA.panel.y,bufferA.panel.w,bufferA.panel.h};
        SDL_Rect panB = {bufferB.panel.x,bufferB.panel.y,bufferB.panel.w,bufferB.panel.h};
        
        SDL_RenderCopy(app.renderer, bufferA.panel.texture, NULL, &panA);
        SDL_RenderCopy(app.renderer, bufferB.panel.texture, NULL, &panB);
        
        
        
        if(app.mode == TEXT_EDIT)
        {
            if(timer.running)
            {
                timer.now = SDL_GetTicks();
                
                if(timer.now < timer.start_time + timer.interval)
                {
                    app.active_buffer->cursor.blink_state_on = false;
                    RenderCursor(app.active_buffer);
                }
                else if(timer.now > timer.start_time + (2 *timer.interval))
                {
                    app.active_buffer->cursor.blink_state_on = true;
                    timer.reset();
                }
            }
            
            if(MarkerIsWithinDrawingBounds(app.active_buffer))
            {
                RenderMarker(app.active_buffer);
            }
        }
        
        SDL_Rect barA = {bufferA.status_bar.x,bufferA.status_bar.y,bufferA.status_bar.w,bufferA.status_bar.h};
        SDL_Rect barB = {bufferB.status_bar.x,bufferB.status_bar.y,bufferB.status_bar.w,bufferB.status_bar.h};
        
        SDL_RenderCopy(app.renderer, bufferA.status_bar.texture, NULL, &barA);
        SDL_RenderCopy(app.renderer, bufferB.status_bar.texture, NULL, &barB);
        
        SDL_RenderPresent(app.renderer);
    }
    
    SDL_FreeCursor(cursor_resize_hor);
    
    XstringDestroy(bufferA.filename);
    XstringDestroy(bufferB.filename);
    XstringDestroy(clipboard.text);
    XstringDestroy(settings.font_name);
    XstringDestroy(settings.start_path);
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
    TTF_CloseFont(font.handle);
    SDL_DestroyRenderer(app.renderer);
    SDL_DestroyWindow(app.window);
    SDL_Quit();
    
    return 0;
}
