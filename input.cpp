#include "input.h"
#include "line.h"
#include "character.h"
#include "draw.h"
#include <iostream>

void GetGlobalInput()
{
    if (app.e.type == SDL_KEYDOWN)
    {
        if (app.e.key.keysym.sym == SDLK_ESCAPE)
        {
            app.quit = true;
        }
    }
};

void GetListNavigationInput()
{
    if (app.e.type == SDL_KEYDOWN)
    {
        if(app.e.key.keysym.sym == SDLK_DOWN)//NOTE: test list input
        {
            app.active_buffer->lst->selected++;
        }
        else if(app.e.key.keysym.sym == SDLK_UP)//NOTE: test list input
        {
            app.active_buffer->lst->selected--;
        }
        else if(app.e.key.keysym.sym == SDLK_RETURN)
        {
            //TEST READ FILE FROM LIST
            char *selected_file = ListGetElement(app.active_buffer->lst, app.active_buffer->lst->selected);
            
            FileReadToBuffer(app.active_buffer, selected_file);
            
            if(app.active_buffer->lst != NULL)
            {
                ListDelete(app.active_buffer->lst);
                app.active_buffer->lst = NULL;
                app.mode = TEXT_EDIT;
                RenderLineRange(app.active_buffer, app.active_buffer->panel.scroll_offset_ver, app.active_buffer->panel.row_capacity, characters_texture, app.active_buffer->panel.texture);
            }
        }
        else if( app.e.key.keysym.sym == SDLK_o && SDL_GetModState() & KMOD_CTRL)
        {
            if(app.active_buffer->lst != NULL)
            {
                ListDelete(app.active_buffer->lst);
                app.active_buffer->lst = NULL;
                app.mode = TEXT_EDIT;
                RenderLineRange(app.active_buffer, app.active_buffer->panel.scroll_offset_ver, app.active_buffer->panel.row_capacity, characters_texture, app.active_buffer->panel.texture);
            }
        }
    }
};

void GetBindedCommandsInput()
{
    if (app.e.type == SDL_KEYDOWN)
    {
        if( app.e.key.keysym.sym == SDLK_s && SDL_GetModState() & KMOD_CTRL)
        {
            FileWriteToDisk(app.active_buffer, "example.txt");
        }
        else if( app.e.key.keysym.sym == SDLK_k && SDL_GetModState() & KMOD_CTRL)
        {
            app.active_buffer->line_node = KillBuffer(app.active_buffer);
        }
        else if( app.e.key.keysym.sym == SDLK_o && SDL_GetModState() & KMOD_CTRL)
        {
            if(app.active_buffer->lst == NULL)
            {
                app.active_buffer->lst = ListCreate("Open:", 100, app.active_buffer->panel.col_capacity - 5);
                PopulateFileList(app.active_buffer->lst, "d:/dev/ed");
                app.mode = LIST_NAV;
            }
        }
        else if( app.e.key.keysym.sym == SDLK_KP_0 && SDL_GetModState() & KMOD_CTRL)
        {
            if(app.active_buffer == &bufferA)
            {
                app.active_buffer = &bufferB;
            }
            else
            {
                app.active_buffer = &bufferA;
            }
        }
    }
};

void GetTextEditingInput()
{
    if(app.e.type == SDL_TEXTINPUT)
    {
        InputText(app.active_buffer);
    }
    else if (app.e.type == SDL_KEYDOWN)
    {
        if(app.e.key.keysym.sym == SDLK_RETURN)
        {
            InputReturn(app.active_buffer);
        }
        else if(app.e.key.keysym.sym == SDLK_BACKSPACE)
        {
            InputBackspace(app.active_buffer);
        }
        else if(app.e.key.keysym.sym == SDLK_DELETE)
        {
            InputDelete(app.active_buffer);
        }
        else if(app.e.key.keysym.sym == SDLK_LEFT)
        {
            InputLeft(app.active_buffer);
        }
        else if(app.e.key.keysym.sym == SDLK_RIGHT)
        {
            InputRight(app.active_buffer);
        }
        else if(app.e.key.keysym.sym == SDLK_UP)
        {
            InputUp(app.active_buffer);
        }
        else if(app.e.key.keysym.sym == SDLK_DOWN)
        {
            InputDown(app.active_buffer);
        }
        else if(app.e.key.keysym.sym == SDLK_HOME)
        {
            InputHome(app.active_buffer);
        }
        else if(app.e.key.keysym.sym == SDLK_END)
        {
            InputEnd(app.active_buffer);
        }
        else if(app.e.key.keysym.sym == SDLK_PAGEUP)
        {
            InputPageUp(app.active_buffer);
        }
        else if(app.e.key.keysym.sym == SDLK_PAGEDOWN)
        {
            InputPageDown(app.active_buffer);
        }
        else if(app.e.key.keysym.sym == SDLK_TAB)
        {
            InputTab(app.active_buffer);
        }
    }
};

void InputText(buffer *buf)
{
    InsertCharacterAt(buf, buf->column);
    RenderCharacterAt(buf, buf->cursor.row, buf->cursor.col - 1, strlen(buf->line_node->data), characters_texture, buf->panel.texture);
    buf->cursor.last_hor_pos = buf->cursor.col;
    
    //SyncCursorWithBuffer(buf);
    SwitchHorizontalPage(buf);
};


void InputReturn(buffer *buf)
{
    if (buf->column == strlen(buf->line_node->data))//cursor at end of line
    {
        buf->column = 0;
        buf->line++;
        SyncCursorWithBuffer(buf);
        InsertLineAt(buf, buf->line);
        
        if (buf->line <  buf->line_count - 1)
        {
            RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
        }
    }
    else// cursor at start or middle of line
    {
        InsertLineAt(buf, buf->line + 1);
        
        int index = 0;
        int len = strlen(buf->line_node->prev->data);
        
        for (int i = buf->column; i < len; ++i)
        {
            buf->line_node->data[index] = buf->line_node->prev->data[i];
            buf->line_node->prev->data[i] = '\0';
            index++;
        }
        
        RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
        
        buf->line++;
        buf->column = 0;
        SyncCursorWithBuffer(buf);
    }
    
    buf->cursor.last_hor_pos = buf->cursor.col;
    
    //test scroll
    if(buf->line_count > buf->panel.row_capacity && buf->cursor.row >= buf->panel.row_capacity)
    {
        buf->panel.scroll_offset_ver++;
        RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
        SyncCursorWithBuffer(buf);
    }
    
    SwitchHorizontalPage(buf);
};


void InputBackspace(buffer *buf)
{
    DeleteCharacterAt(buf, buf->column);
    RenderClearCharacterAt(buf, buf->cursor.row, buf->cursor.col, strlen(buf->line_node->data),characters_texture, buf->panel.texture);
    buf->cursor.last_hor_pos = buf->cursor.col;
    
    //test scroll
    if(buf->panel.scroll_offset_ver > 0 && buf->panel.scroll_offset_ver <= (buf->line_count - buf->panel.row_capacity))
    {
        RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
    }
    else if(buf->panel.scroll_offset_ver > 0 && buf->panel.scroll_offset_ver > (buf->line_count - buf->panel.row_capacity))
    {
        buf->panel.scroll_offset_ver--;
        RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
    }
    
    SwitchHorizontalPage(buf);
    SyncCursorWithBuffer(buf);
};


void InputDelete(buffer *buf)
{
    if(buf->column < strlen(buf->line_node->data))
    {
        buf->column++;
        SyncCursorWithBuffer(buf);
        DeleteCharacterAt(buf, buf->column);
        RenderClearCharacterAt(buf, buf->cursor.row, buf->cursor.col, strlen(buf->line_node->data),characters_texture, buf->panel.texture);
    }
    else if(buf->line < buf->line_count - 1)
    {
        buf->line++;
        buf->column = 0;
        SyncCursorWithBuffer(buf);
        buf->line_node = buf->line_node->next;
        DeleteCharacterAt(buf, buf->column);
        RenderClearCharacterAt(buf, buf->cursor.row, buf->cursor.col, strlen(buf->line_node->data),characters_texture, buf->panel.texture);
    }
    buf->cursor.last_hor_pos = buf->cursor.col;
    
    //test scroll
    if(buf->panel.scroll_offset_ver > 0 && buf->panel.scroll_offset_ver <= (buf->line_count - buf->panel.row_capacity))
    {
        RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
    }
    else if(buf->panel.scroll_offset_ver > 0 && buf->panel.scroll_offset_ver > (buf->line_count - buf->panel.row_capacity))
    {
        buf->panel.scroll_offset_ver--;
        RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
    }
    
    SwitchHorizontalPage(buf);
    SyncCursorWithBuffer(buf);
};


void InputLeft(buffer *buf)
{
    if(buf->column > 0)
    {
        buf->column--;
    }
    else
    {
        if(buf->line_node->prev != buf->head)
        {
            buf->line_node = buf->line_node->prev;
            buf->line--;
            buf->column = strlen(buf->line_node->data);
        }
    }
    
    SwitchHorizontalPage(buf);
    SyncCursorWithBuffer(buf);
    buf->cursor.last_hor_pos = buf->cursor.col;
    
    //test scroll
    if(buf->line_count > buf->panel.row_capacity && buf->cursor.row < 0)
    {
        buf->panel.scroll_offset_ver--;
        RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
        SyncCursorWithBuffer(buf);
    }
};


void InputRight(buffer *buf)
{
    if(buf->column < strlen(buf->line_node->data))
    {
        buf->column++;
    }
    else
    {
        if(buf->line_node->next != NULL)
        {
            buf->line_node = buf->line_node->next;
            buf->line++;
            buf->column = 0;
        }
    }
    
    SwitchHorizontalPage(buf);
    SyncCursorWithBuffer(buf);
    buf->cursor.last_hor_pos = buf->cursor.col;
    
    //test scroll
    if(buf->line_count > buf->panel.row_capacity && buf->cursor.row >= buf->panel.row_capacity)
    {
        buf->panel.scroll_offset_ver++;
        RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
        SyncCursorWithBuffer(buf);
    }
};


void InputUp(buffer *buf)
{
    if(buf->line > 0)
    {
        buf->line--;
        SyncCursorWithBuffer(buf);
        buf->line_node = buf->line_node->prev;
        AttemptSetToLastColumn(buf);
        SwitchHorizontalPage(buf);
    }
    else
    {
        buf->column = 0;
        SwitchHorizontalPage(buf);
        SyncCursorWithBuffer(buf);
    }
    
    //test scroll
    if(buf->line_count > buf->panel.row_capacity && buf->cursor.row < 0)
    {
        buf->panel.scroll_offset_ver--;
        RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
        SyncCursorWithBuffer(buf);
    }
    
    SyncCursorWithBuffer(buf);
};


void InputDown(buffer *buf)
{
    if(buf->line < buf->line_count - 1)
    {
        buf->line++;
        SyncCursorWithBuffer(buf);
        buf->line_node = buf->line_node->next;
        AttemptSetToLastColumn(buf);
        SwitchHorizontalPage(buf);
    }
    else
    {
        buf->column = strlen(buf->line_node->data);
        SwitchHorizontalPage(buf);
        SyncCursorWithBuffer(buf);
    }
    
    //test scroll
    if(buf->line_count > buf->panel.row_capacity && buf->cursor.row >= buf->panel.row_capacity)
    {
        buf->panel.scroll_offset_ver++;
        RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
        SyncCursorWithBuffer(buf);
    }
    
    SyncCursorWithBuffer(buf);
};


void InputTab(buffer *buf)
{
    for (int i = 0; i < 4; ++i)
    {
        U8_strinsert(buf->line_node->data, buf->column, " ", 256);
        buf->column++;
        SyncCursorWithBuffer(buf);
        RenderCharacterAt(buf, buf->cursor.row, buf->cursor.col - 1, strlen(buf->line_node->data), characters_texture, buf->panel.texture);
        buf->cursor.last_hor_pos = buf->cursor.col;
    }
};

void InputHome(buffer *buf)
{
    if(buf->column > 0)
    {
        buf->column = 0;
        buf->cursor.last_hor_pos = 0;
        SwitchHorizontalPage(buf);
        SyncCursorWithBuffer(buf);
    }
};

void InputEnd(buffer *buf)
{
    int len = strlen(buf->line_node->data);
    buf->column = len;
    buf->cursor.last_hor_pos = len;
    SwitchHorizontalPage(buf);
    SyncCursorWithBuffer(buf);
};

void InputPageUp(buffer *buf)
{
    if(buf->line_count > buf->panel.row_capacity && buf->line > buf->panel.row_capacity)
    {
        for (int i = buf->line; i > buf->line - buf->panel.row_capacity; --i)
        {
            buf->line_node = buf->line_node->prev;
        }
        
        buf->line -= buf->panel.row_capacity;
        SyncCursorWithBuffer(buf);
        
        //scroll
        if(buf->line_count > buf->panel.row_capacity && buf->cursor.row < 0)
        {
            buf->panel.scroll_offset_ver -= buf->panel.row_capacity;
            
            if(buf->panel.scroll_offset_ver < 0)
            {
                buf->panel.scroll_offset_ver= 0;
            }
            
            RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
        }
    }
    else
    {
        buf->line_node = buf->head->next;
        buf->line = 0;
        buf->panel.scroll_offset_ver = 0;
        RenderLineRange(buf, 0, buf->panel.row_capacity, characters_texture, buf->panel.texture);
    }
    
    AttemptSetToLastColumn(buf);
    SwitchHorizontalPage(buf);
    SyncCursorWithBuffer(buf);
};

void InputPageDown(buffer *buf)
{
    if(buf->line_count > buf->panel.row_capacity && buf->line < buf->line_count - buf->panel.row_capacity)
    {
        for (int i = buf->line; i < buf->line + buf->panel.row_capacity; ++i)
        {
            buf->line_node = buf->line_node->next;
        }
        
        buf->line += buf->panel.row_capacity;
        
        //test scroll
        int complete_pages = (buf->line_count / buf->panel.row_capacity) * buf->panel.row_capacity;
        if(buf->line < complete_pages)
        {
            buf->panel.scroll_offset_ver += buf->panel.row_capacity;
            RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
        }
        else
        {
            buf->panel.scroll_offset_ver = buf->line_count - buf->panel.row_capacity;
            RenderLineRange(buf, buf->line_count - buf->panel.row_capacity, buf->panel.row_capacity, characters_texture, buf->panel.texture);
        }
    }
    else
    {
        while(buf->line_node->next != NULL)
        {
            buf->line_node = buf->line_node->next;
        }
        
        buf->line = buf->line_count-1;
        buf->panel.scroll_offset_ver = buf->line_count - buf->panel.row_capacity;
        RenderLineRange(buf, buf->line_count - buf->panel.row_capacity, buf->panel.row_capacity, characters_texture, buf->panel.texture);
    }
    
    AttemptSetToLastColumn(buf);
    SwitchHorizontalPage(buf);
    SyncCursorWithBuffer(buf);
};

void ListNav_Down(list *l)
{
    
};