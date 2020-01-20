#include "input.h"

void ProcessInput_Global()
{
    //nothing here for the time being
};

void ProcessInput_ListNavigation()
{
    if(app.e.type == SDL_TEXTINPUT)
    {
        XstringConcat(app.active_buffer->lst->filter, 1, app.e.text.text);
        char *selected_element = ListGetElement(app.active_buffer->lst, app.active_buffer->lst->selected);
        
        if(XstringGetLength(app.active_buffer->lst->current_path) > 0) // haven't reached top level dir
        {
#ifdef _WIN32
            if(app.e.text.text[0] == 92)//Backslash
#endif
#ifdef __linux__
                if(app.e.text.text[0] == 47)//Slash
#endif
            {
                if(IsValidPathFilter(XstringGet(app.active_buffer->lst->current_path), XstringGet(app.active_buffer->lst->filter)))
                {
                    ListSwitchToSelectedDirectory(app.active_buffer->lst, selected_element);
                }
                else
                {
                    ListClear(app.active_buffer->lst);
                    ListApplyFilter(app.active_buffer->lst, XstringGet(app.active_buffer->lst->current_path));
                }
            }
            else
            {
                ListClear(app.active_buffer->lst);
                ListApplyFilter(app.active_buffer->lst, XstringGet(app.active_buffer->lst->current_path));
            }
        }
        else// reached top level dir
        {
#ifdef _WIN32
            if(app.e.text.text[0] == 92)//Backslash
#endif
#ifdef __linux__
                if(app.e.text.text[0] == 47)//Slash
#endif
            {
                XstringSet(app.last_path, XstringGet(app.active_buffer->lst->filter));
                
                ListSwitchToSelectedDirectory(app.active_buffer->lst, XstringGet(app.active_buffer->lst->filter));
            }
        }
        
    }
    else if (app.e.type == SDL_KEYDOWN)
    {
        if(app.e.key.keysym.sym == SDLK_DOWN)
        {
            Input_ListNav_Down(app.active_buffer->lst);
        }
        else if(app.e.key.keysym.sym == SDLK_UP)
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
            if(XstringGetLength(app.active_buffer->lst->current_path) > 0) // haven't reached top level dir
            {
                if(XstringGetLength(app.active_buffer->lst->filter) == 0)
                {
                    Input_ListNav_ParentDirectory(app.active_buffer->lst);
                }
                else
                {
                    if(XstringGetLength(app.active_buffer->lst->filter) == 1)
                    {
                        XstringSet(app.active_buffer->lst->filter, "");
                    }
                    XstringTruncateTail(app.active_buffer->lst->filter, 1);
                    
                    ListClear(app.active_buffer->lst);
                    ListApplyFilter(app.active_buffer->lst, XstringGet(app.active_buffer->lst->current_path));
                }
            }
            else if(XstringGetLength(app.active_buffer->lst->current_path) == 0 && XstringGetLength(app.active_buffer->lst->filter) > 0)// reached top level dir
            {
                XstringSet(app.last_path, XstringGet(app.active_buffer->lst->current_path));
                XstringTruncateTail(app.active_buffer->lst->filter, 1);
            }
        }
    }
};

void ProcessInput_Commands()
{
    if (app.e.type == SDL_KEYDOWN)
    {
        if(CommandKeyBinding(settings.kb_command_file_save))
        {
            FileWriteToDisk(app.active_buffer, "example.txt");
        }
        else if(CommandKeyBinding(settings.kb_command_marker_set))
        {
            app.active_buffer->marker.row = app.active_buffer->line;
            app.active_buffer->marker.col = app.active_buffer->column;
        }
        else if(CommandKeyBinding(settings.kb_command_buffer_kill))
        {
            app.active_buffer->line_node = KillBuffer(app.active_buffer);
        }
        else if(CommandKeyBinding(settings.kb_command_file_open))
        {
            if(app.active_buffer->lst == NULL)
            {
                app.active_buffer->lst = ListCreate("Open:", 128, 260);
                ListPopulate(app.active_buffer->lst, XstringGet(app.last_path));
                app.mode = LIST_NAV;
            }
        }
        else if(CommandKeyBinding(settings.kb_command_buffer_toggle_active))
        {
            if(app.active_buffer == &bufferA)
            {
                app.active_buffer = &bufferB;
            }else
            {
                app.active_buffer = &bufferA;
            }
        }
    }
};

void ProcessInput_TextEditing()
{
    const SDL_Keymod modkeys = (SDL_Keymod)(KMOD_CTRL | KMOD_ALT | KMOD_GUI);
    const bool no_mod_keys{(SDL_GetModState() & modkeys) == KMOD_NONE};
    
    bool CTRL_ONLY = app.e.key.keysym.mod & KMOD_CTRL &&
        !(app.e.key.keysym.mod & (KMOD_ALT | KMOD_SHIFT));
    bool SHIFT_ONLY = app.e.key.keysym.mod & KMOD_SHIFT &&
        !(app.e.key.keysym.mod & (KMOD_CTRL | KMOD_ALT));
    bool ALT_ONLY = app.e.key.keysym.mod & KMOD_ALT &&
        !(app.e.key.keysym.mod & (KMOD_CTRL | KMOD_SHIFT));
    bool CTRL_SHIFT = app.e.key.keysym.mod & KMOD_CTRL && app.e.key.keysym.mod & KMOD_SHIFT;
    bool CTRL_ALT = app.e.key.keysym.mod & KMOD_CTRL && app.e.key.keysym.mod & KMOD_ALT;
    bool ALT_SHIFT = app.e.key.keysym.mod & KMOD_ALT && app.e.key.keysym.mod & KMOD_SHIFT;
    
    
    
    if(app.e.type == SDL_TEXTINPUT && no_mod_keys)
    {
        Input_TextEd_Text(app.active_buffer);
    }
    else if (app.e.type == SDL_KEYDOWN)
    {
        if(app.e.key.keysym.sym == SDLK_RETURN || app.e.key.keysym.sym == SDLK_RETURN2)
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
        else if( app.e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL)
        {
            ClipBoardCopy(app.active_buffer);
            char *temp = SDL_GetClipboardText();
        }
        else if( app.e.key.keysym.sym == SDLK_x && SDL_GetModState() & KMOD_CTRL)
        {
            ClipBoardCut(app.active_buffer);
        }
        else if( app.e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL)
        {
            char *temp = SDL_GetClipboardText();
            ClipBoardPaste(app.active_buffer);
        }
        else if( app.e.key.keysym.sym == SDLK_z && CTRL_ONLY)
        {
            //UndoStackCommitUndo(app.active_buffer);
        }
        else if(app.e.key.keysym.sym == SDLK_z && CTRL_SHIFT)
        {
            //UndoStackCommitRedo(app.active_buffer);
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
    
    CharacterInsert(buf, buf->column);
    RenderCharacter(buf, buf->cursor.row, buf->cursor.col - 1, strlen(buf->line_node->data), characters_texture, buf->panel.texture);
    buf->cursor.last_hor_pos = buf->cursor.col;
    
    SwitchHorizontalPage(buf);
};


void Input_TextEd_Return(buffer *buf)
{
    //Store current identation level
    int len = strlen(buf->line_node->data);
    int identation_level = 0;
    char ch = ' ';
    
    if(len > 0)
    {
        while(buf->line_node->data[identation_level] == ch)
        {
            if(identation_level < len)
            {
                identation_level++;
            }
        }
    }
    
    
    if (buf->column == strlen(buf->line_node->data))//cursor at end of line
    {
        if(buf->marker.row > buf->line)
        {
            buf->marker.row++;
        }
        
        buf->column = 0;
        buf->line++;
        SyncCursorWithBuffer(buf);
        LineInsert(buf, buf->line);
        
        if(identation_level > 0)
        {
            memset(buf->line_node->data, ch, identation_level);
            buf->column = identation_level;
            buf->cursor.col = identation_level;
        }
        
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
        
        LineInsert(buf, buf->line + 1);
        
        int len = strlen(buf->line_node->prev->data);
        
        if(len - buf->column + identation_level > settings.line_mem_chunk)
        {
            int chunks_to_ask = ((len - buf->column -  + identation_level - settings.line_mem_chunk) / settings.line_mem_chunk) + 1;
            LineRequestMemChunks(buf->line_node, chunks_to_ask);
        }
        
        if(identation_level > 0)
        {
            memset(buf->line_node->data, ch, identation_level);
        }
        
        int index = 0 + identation_level;
        
        for (int i = buf->column; i < len; ++i)
        {
            buf->line_node->data[index] = buf->line_node->prev->data[i];
            buf->line_node->prev->data[i] = '\0';
            index++;
        }
        
        LineShrinkMemChunks(buf->line_node->prev);
        
        RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
        
        buf->line++;
        buf->column = identation_level;
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
    CharacterDelete(buf, buf->column);
    RenderClearCharacter(buf, buf->cursor.row, buf->cursor.col, strlen(buf->line_node->data),characters_texture, buf->panel.texture);
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
        CharacterDelete(buf, buf->column);
        RenderClearCharacter(buf, buf->cursor.row, buf->cursor.col, strlen(buf->line_node->data),characters_texture, buf->panel.texture);
    }
    else if(buf->line < buf->line_count - 1)
    {
        buf->line++;
        buf->column = 0;
        SyncCursorWithBuffer(buf);
        buf->line_node = buf->line_node->next;
        CharacterDelete(buf, buf->column);
        RenderClearCharacter(buf, buf->cursor.row, buf->cursor.col, strlen(buf->line_node->data),characters_texture, buf->panel.texture);
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
    for (int i = 0; i < settings.tab_size; ++i)
    {
        LineEnsureSufficientCapacity(buf->line_node);
        if(buf->column < strlen(buf->line_node->data))
        {
            int char_count = strlen(buf->line_node->data) - buf->column;
            memmove(buf->line_node->data + buf->column + 1, buf->line_node->data + buf->column, char_count);
        }
        
        *(buf->line_node->data + buf->column) = ' ';
        buf->column++;
        SyncCursorWithBuffer(buf);
        RenderCharacter(buf, buf->cursor.row, buf->cursor.col - 1, strlen(buf->line_node->data), characters_texture, buf->panel.texture);
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
        ListPopulate(l, XstringGet(l->current_path));
    }
    else
    {
        //XstringTruncateTail(l->current_path, 1);
        XstringSet(l->current_path, "");
#ifdef _WIN32
        //int pos = XstringIndexOfLastOccurrance(l->current_path, '\\') + 1;
#endif
#ifdef __linux__
        //int pos = XstringIndexOfLastOccurrance(l->current_path, '/') + 1;
#endif
        //XstringTruncateTail(l->current_path, XstringGetLength(l->current_path) - pos);
        ListClear(l);
        
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

void Input_Scroll(buffer *buf)
{
    if(buf->line_count <= buf->panel.row_capacity)
    {
        return;
    }
    
    if(app.e.wheel.y > 0) // scroll up
    {
        int wheel_y = app.e.wheel.y;
        int diff = 0;
        
        if(buf->cursor.row >= buf->panel.row_capacity - 1)
        {
            diff = 1;
        }
        
        if(wheel_y > 1)
        {
            if(buf->cursor.row >= buf->panel.row_capacity - 1)
            {
                diff = wheel_y;
            }
            else
            {
                int rows_left = buf->panel.row_capacity - buf->cursor.row-1;
                
                if(wheel_y > rows_left)
                {
                    diff = wheel_y - rows_left;
                }
                
                if(diff > buf->panel.scroll_offset_ver)
                {
                    diff = buf->panel.scroll_offset_ver;
                }
            }
        }
        
        for (int i = 0; i < diff; ++i)
        {
            buf->line_node = buf->line_node->prev;
            buf->line--;
        }
        
        if(wheel_y < buf->panel.scroll_offset_ver)
        {
            buf->panel.scroll_offset_ver -= wheel_y;
        }
        else
        {
            buf->panel.scroll_offset_ver = 0;
        }
        
        SyncCursorWithBuffer(buf);
        RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
    }
    else if(app.e.wheel.y < 0) // scroll down
    {
        int wheel_y = (app.e.wheel.y * -1);
        int diff = 0;
        
        if(wheel_y > buf->cursor.row)
        {
            diff = wheel_y - buf->cursor.row;
        }
        
        buf->panel.scroll_offset_ver += wheel_y;
        
        if(buf->panel.scroll_offset_ver > buf->line_count - buf->panel.row_capacity)
        {
            buf->panel.scroll_offset_ver = buf->line_count - buf->panel.row_capacity;
        }
        
        for (int i = 0; i < diff; ++i)
        {
            if(buf->line_node->next != NULL)
            {
                buf->line_node = buf->line_node->next;
                buf->line++;
            }
        }
        
        SyncCursorWithBuffer(buf);
        RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
    }
    
    if(app.e.wheel.x > 0) // scroll right
    {
        buf->panel.page++;
        SyncCursorWithBuffer(buf);
        RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
    }
    else if(app.e.wheel.x < 0) // scroll left
    {
        buf->panel.page--;
        SyncCursorWithBuffer(buf);
        RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
    }
};

void CursorAnchorToBounds(buffer *buf)
{
    
};