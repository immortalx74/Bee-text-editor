#include "input.h"
#include "line.h"
#include "character.h"
#include "draw.h"
#include <iostream>

void GetGlobalInput()
{
    if (app.e.type == SDL_KEYDOWN)
    {
        //if (app.e.key.keysym.sym == SDLK_ESCAPE)
        //{
        //app.quit = true;
        //}
    }
};

void GetListNavigationInput()
{
    if(app.e.type == SDL_TEXTINPUT)
    {
        XstringConcat(app.active_buffer->lst->filter, 2, XstringGet(app.active_buffer->lst->filter), app.e.text.text);
        char *selected_element = ListGetElement(app.active_buffer->lst, app.active_buffer->lst->selected);
        
        if(XstringGetLength(app.last_path) > 0) // haven't reached top level dir
        {
#ifdef _WIN32
            if(app.e.text.text[0] == 92)//Backslash
#endif
#ifdef __linux__
                if(app.e.text.text[0] == 47)//Backslash
#endif
            {
                if(IsValidPathFilter(XstringGet(app.active_buffer->lst->current_path), XstringGet(app.active_buffer->lst->filter)))
                {
                    ListSwitchToSelectedDirectory(app.active_buffer->lst, selected_element);
                }
                else
                {
                    ListClear(app.active_buffer->lst);
                    FilterFileList(app.active_buffer->lst, XstringGet(app.active_buffer->lst->current_path));
                }
            }
            else
            {
                ListClear(app.active_buffer->lst);
                FilterFileList(app.active_buffer->lst, XstringGet(app.active_buffer->lst->current_path));
            }
        }
        else// reached top level dir
        {
#ifdef _WIN32
            if(app.e.text.text[0] == 92)//Backslash
#endif
#ifdef __linux__
                if(app.e.text.text[0] == 47)//Backslash
#endif
            {
                XstringSet(app.last_path, XstringGet(app.active_buffer->lst->filter));
                
                ListSwitchToSelectedDirectory(app.active_buffer->lst, XstringGet(app.active_buffer->lst->filter));
            }
        }
        
    }
    else if (app.e.type == SDL_KEYDOWN)
    {
        if(app.e.key.keysym.sym == SDLK_DOWN)//NOTE: test list input
        {
            Input_ListNav_Down(app.active_buffer->lst);
        }
        else if(app.e.key.keysym.sym == SDLK_UP)//NOTE: test list input
        {
            Input_ListNav_Up(app.active_buffer->lst);
        }
        else if(app.e.key.keysym.sym == SDLK_RETURN)
        {
            Input_ListNav_Select(app.active_buffer->lst);
        }
        else if( app.e.key.keysym.sym == SDLK_ESCAPE)
        {
            Input_ListNav_Abort(app.active_buffer->lst);
        }
        else if( app.e.key.keysym.sym == SDLK_BACKSPACE)
        {
            if(XstringGetLength(app.last_path) > 0) // haven't reached top level dir
            {
                if(XstringGetLength(app.active_buffer->lst->filter) == 0)
                {
                    Input_ListNav_ParentDirectory(app.active_buffer->lst);
                }
                else
                {
                    XstringTruncateTail(app.active_buffer->lst->filter, 1);
                    
                    ListClear(app.active_buffer->lst);
                    FilterFileList(app.active_buffer->lst, XstringGet(app.active_buffer->lst->current_path));
                }
            }
            else if(XstringGetLength(app.last_path) == 0 && XstringGetLength(app.active_buffer->lst->filter) > 0)// reached top level dir
            {
                XstringTruncateTail(app.active_buffer->lst->filter, 1);
            }
            else if(XstringGetLength(app.last_path) == 0 && XstringGetLength(app.active_buffer->lst->filter) == 0)//test remove later
            {
                std::cout << XstringGet(app.last_path) << std::endl;
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
        else if( app.e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL)
        {
            clipboard.text = XstringCreate("");
            int start_line = MIN(app.active_buffer->line, app.active_buffer->marker.row);
            int end_line = MAX(app.active_buffer->line, app.active_buffer->marker.row);
            int line_diff = end_line - start_line;
            
            clipboard.line_lengths = (int*)malloc((line_diff + 1) * sizeof(int));
            
            int trim_left, trim_right;
            
            if (start_line == app.active_buffer->line)
            {
                trim_left = app.active_buffer->column;
                node *temp = app.active_buffer->line_node;
                
                for (int i = 0; i < line_diff; ++i)
                {
                    temp = temp->next;
                }
                
                trim_right = strlen(temp->data) - app.active_buffer->marker.col;
            }
            else
            {
                trim_left = app.active_buffer->marker.col;
                trim_right = strlen(app.active_buffer->line_node->data) - app.active_buffer->column;
            }
            
            if(app.active_buffer->line < app.active_buffer->marker.row)//cursor first
            {
                node *cur = app.active_buffer->line_node;
                
                for (int i = 0; i <= line_diff; ++i)
                {
                    clipboard.line_lengths[i] = strlen(cur->data);
                    
                    if(i == 0)
                    {
                        clipboard.line_lengths[i] -= trim_left;
                    }
                    if(i == line_diff)
                    {
                        clipboard.line_lengths[i] -= trim_right;
                    }
                    
                    XstringConcat(clipboard.text, 2, XstringGet(clipboard.text), cur->data);
                    cur = cur->next;
                }
                
                XstringTruncateHead(clipboard.text, trim_left);
                XstringTruncateTail(clipboard.text, trim_right);
            }
            else if(app.active_buffer->line > app.active_buffer->marker.row)//marker first
            {
                node *cur = app.active_buffer->line_node;
                
                // Firstly move to the node where the marker is at
                for (int i = 0; i < line_diff; ++i)
                {
                    cur = cur->prev;
                }
                
                for (int i = 0; i <= line_diff; ++i)
                {
                    clipboard.line_lengths[i] = strlen(cur->data);
                    
                    if(i == 0)
                    {
                        clipboard.line_lengths[i] -= trim_left;
                    }
                    if(i == line_diff)
                    {
                        clipboard.line_lengths[i] -= trim_right;
                    }
                    
                    XstringConcat(clipboard.text, 2, XstringGet(clipboard.text), cur->data);
                    cur = cur->next;
                }
                
                XstringTruncateHead(clipboard.text, trim_left);
                XstringTruncateTail(clipboard.text, trim_right);
            }
            else if(app.active_buffer->line == app.active_buffer->marker.row)//cursor row = marker row
            {
                if(app.active_buffer->column != app.active_buffer->marker.col)
                {
                    node *cur = app.active_buffer->line_node;
                    XstringSet(clipboard.text, cur->data);
                    
                    int col_left = MIN(app.active_buffer->column, app.active_buffer->marker.col);
                    int col_right = MAX(app.active_buffer->column, app.active_buffer->marker.col);
                    col_right= strlen(cur->data) - col_right;
                    XstringTruncateHead(clipboard.text, col_left);
                    XstringTruncateTail(clipboard.text, col_right);
                }
                else//copy single character
                {
                    char *singlechar = (char*)calloc(2, 1);
                    memcpy(singlechar, app.active_buffer->line_node->data + app.active_buffer->column, 1);
                    XstringSet(clipboard.text, singlechar);
                    free(singlechar);
                }
            }
            
            SDL_SetClipboardText(XstringGet(clipboard.text));
            XstringDestroy(clipboard.text);
        }
        else if( app.e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL)
        {
            char *temp = SDL_GetClipboardText();
            std::cout << temp << std::endl;
        }
        else if( app.e.key.keysym.sym == SDLK_SPACE && SDL_GetModState() & KMOD_CTRL)
        {
            //set marker
            app.active_buffer->marker.row = app.active_buffer->line;
            app.active_buffer->marker.col = app.active_buffer->column;
        }
        else if( app.e.key.keysym.sym == SDLK_k && SDL_GetModState() & KMOD_CTRL)
        {
            app.active_buffer->line_node = KillBuffer(app.active_buffer);
        }
        else if( app.e.key.keysym.sym == SDLK_o && SDL_GetModState() & KMOD_CTRL)
        {
            if(app.active_buffer->lst == NULL)
            {
                app.active_buffer->lst = ListCreate("Open:", 400, app.active_buffer->panel.col_capacity - 5);
                PopulateFileList(app.active_buffer->lst, XstringGet(app.last_path));
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
    const SDL_Keymod modkeys = (SDL_Keymod)(KMOD_CTRL | KMOD_SHIFT | KMOD_ALT | KMOD_GUI);
    const bool no_mod_keys{(SDL_GetModState() & modkeys) == KMOD_NONE};
    
    if(app.e.type == SDL_TEXTINPUT && no_mod_keys)
    {
        Input_TextEd_Text(app.active_buffer);
    }
    else if (app.e.type == SDL_KEYDOWN)
    {
        if(app.e.key.keysym.sym == SDLK_RETURN)
        {
            Input_TextEd_Return(app.active_buffer);
        }
        else if(app.e.key.keysym.sym == SDLK_BACKSPACE)
        {
            Input_TextEd_Backspace(app.active_buffer);
        }
        else if(app.e.key.keysym.sym == SDLK_DELETE)
        {
            Input_TextEd_Delete(app.active_buffer);
        }
        else if(app.e.key.keysym.sym == SDLK_LEFT)
        {
            Input_TextEd_Left(app.active_buffer);
        }
        else if(app.e.key.keysym.sym == SDLK_RIGHT)
        {
            Input_TextEd_Right(app.active_buffer);
        }
        else if(app.e.key.keysym.sym == SDLK_UP)
        {
            Input_TextEd_Up(app.active_buffer);
        }
        else if(app.e.key.keysym.sym == SDLK_DOWN)
        {
            Input_TextEd_Down(app.active_buffer);
        }
        else if(app.e.key.keysym.sym == SDLK_HOME)
        {
            Input_TextEd_Home(app.active_buffer);
        }
        else if(app.e.key.keysym.sym == SDLK_END)
        {
            Input_TextEd_End(app.active_buffer);
        }
        else if(app.e.key.keysym.sym == SDLK_PAGEUP)
        {
            Input_TextEd_PageUp(app.active_buffer);
        }
        else if(app.e.key.keysym.sym == SDLK_PAGEDOWN)
        {
            Input_TextEd_PageDown(app.active_buffer);
        }
        else if(app.e.key.keysym.sym == SDLK_TAB)
        {
            Input_TextEd_Tab(app.active_buffer);
        }
    }
};

void Input_TextEd_Text(buffer *buf)
{
    //marker update
    if(buf->marker.row == buf->line)
    {
        if(buf->marker.col > buf->column)
        {
            buf->marker.col++;
        }
    }
    
    InsertCharacterAt(buf, buf->column);
    RenderCharacterAt(buf, buf->cursor.row, buf->cursor.col - 1, strlen(buf->line_node->data), characters_texture, buf->panel.texture);
    buf->cursor.last_hor_pos = buf->cursor.col;
    
    SwitchHorizontalPage(buf);
};


void Input_TextEd_Return(buffer *buf)
{
    if (buf->column == strlen(buf->line_node->data))//cursor at end of line
    {
        if(buf->marker.row > buf->line)
        {
            buf->marker.row++;
        }
        
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
        if(buf->marker.row > buf->line)
        {
            buf->marker.row++;
        }
        else if(buf->marker.row >= buf->line && buf->column < buf->marker.col)
        {
            buf->marker.row++;
            buf->marker.col -= buf->column;
        }
        
        InsertLineAt(buf, buf->line + 1);
        
        int index = 0;
        int len = strlen(buf->line_node->prev->data);
        
        if(len - buf->column > LINE_MEM_CHUNK)
        {
            int chunks_to_ask = ((len - buf->column - LINE_MEM_CHUNK) / LINE_MEM_CHUNK) + 1;
            LineRequestMemChunks(buf->line_node, chunks_to_ask);
        }
        
        for (int i = buf->column; i < len; ++i)
        {
            buf->line_node->data[index] = buf->line_node->prev->data[i];
            buf->line_node->prev->data[i] = '\0';
            index++;
        }
        
        LineShrinkMemChunks(buf->line_node->prev);
        
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


void Input_TextEd_Backspace(buffer *buf)
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


void Input_TextEd_Delete(buffer *buf)
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


void Input_TextEd_Left(buffer *buf)
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


void Input_TextEd_Right(buffer *buf)
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


void Input_TextEd_Up(buffer *buf)
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


void Input_TextEd_Down(buffer *buf)
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


void Input_TextEd_Tab(buffer *buf)
{
    for (int i = 0; i < 4; ++i)
    {
        U8_strinsert(buf->line_node->data, buf->column, " ", LINE_MEM_CHUNK);
        buf->column++;
        SyncCursorWithBuffer(buf);
        RenderCharacterAt(buf, buf->cursor.row, buf->cursor.col - 1, strlen(buf->line_node->data), characters_texture, buf->panel.texture);
        buf->cursor.last_hor_pos = buf->cursor.col;
    }
};

void Input_TextEd_Home(buffer *buf)
{
    if(buf->column > 0)
    {
        buf->column = 0;
        buf->cursor.last_hor_pos = 0;
        SwitchHorizontalPage(buf);
        SyncCursorWithBuffer(buf);
    }
};

void Input_TextEd_End(buffer *buf)
{
    int len = strlen(buf->line_node->data);
    buf->column = len;
    buf->cursor.last_hor_pos = len;
    SwitchHorizontalPage(buf);
    SyncCursorWithBuffer(buf);
};

void Input_TextEd_PageUp(buffer *buf)
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

void Input_TextEd_PageDown(buffer *buf)
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
        
        if(buf->line_count > buf->panel.row_capacity)
        {
            buf->panel.scroll_offset_ver = buf->line_count - buf->panel.row_capacity;
            RenderLineRange(buf, buf->line_count - buf->panel.row_capacity, buf->panel.row_capacity, characters_texture, buf->panel.texture);
        }
        else
        {
            RenderLineRange(buf, 0, buf->line_count, characters_texture, buf->panel.texture);
        }
    }
    
    AttemptSetToLastColumn(buf);
    SwitchHorizontalPage(buf);
    SyncCursorWithBuffer(buf);
};

void Input_ListNav_Down(list *l)
{
    if(l->selected < l->element_count - 1)
    {
        l->selected++;
        
        if (l->element_count > app.active_buffer->panel.row_capacity && l->row >= app.active_buffer->panel.row_capacity-1)
        {
            l->scroll_offset++;
        }
        l->row = l->selected - l->scroll_offset;
    }
};

void Input_ListNav_Up(list *l)
{
    if(l->selected > 0)
    {
        l->selected--;
        l->row = l->selected - l->scroll_offset;
        
        if(l->element_count > app.active_buffer->panel.row_capacity && l->row == 0 && l->selected > 0)
        {
            l->scroll_offset--;
        }
    }
};

void Input_ListNav_Select(list *l)
{
    char *selected_element = ListGetElement(app.active_buffer->lst, app.active_buffer->lst->selected);
    
    if (IsDirectory(selected_element))
    {
        ListSwitchToSelectedDirectory(l, selected_element);
    }
    else
    {
        if(selected_element[0] != 0)
        {
            ListLoadSelectedFile(l, selected_element);
        }
    }
};

void Input_ListNav_ParentDirectory(list *l)
{
    if(!IsTopLevelDirectory(l->current_path))
    {
        XstringTruncateTail(l->current_path, 1);
#ifdef _WIN32
        int pos = XstringIndexOfLastOccurrance(l->current_path, '\\') + 1;
#endif
#ifdef __linux__
        int pos = XstringIndexOfLastOccurrance(l->current_path, '/') + 1;
#endif
        XstringTruncateTail(l->current_path, XstringGetLength(l->current_path) - pos);
        
        ListClear(l);
        PopulateFileList(l, XstringGet(l->current_path));
    }
    else
    {
        XstringTruncateTail(l->current_path, 1);
#ifdef _WIN32
        int pos = XstringIndexOfLastOccurrance(l->current_path, '\\') + 1;
#endif
#ifdef __linux__
        int pos = XstringIndexOfLastOccurrance(l->current_path, '/') + 1;
#endif
        XstringTruncateTail(l->current_path, XstringGetLength(l->current_path) - pos);
        ListClear(l);
        XstringSet(app.last_path, "");
    }
};

void Input_ListNav_Abort(list *l)
{
    if(app.active_buffer->lst != NULL)
    {
        ListDelete(app.active_buffer->lst);
        app.active_buffer->lst = NULL;
        app.mode = TEXT_EDIT;
        RenderLineRange(app.active_buffer, app.active_buffer->panel.scroll_offset_ver, app.active_buffer->panel.row_capacity, characters_texture, app.active_buffer->panel.texture);
    }
};