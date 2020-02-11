#include "clipboard.h"

void ClipBoardCopy(buffer *buf)
{
    if(buf->column == buf->marker.col && buf->line == buf->marker.row)
    {
        return;
    }
    
    int start_line = MIN(buf->line, buf->marker.row);
    int end_line = MAX(buf->line, buf->marker.row);
    int line_diff = end_line - start_line;
    int start_col, end_col;
    
    if(buf->line != buf->marker.row)
    {
        if(start_line == buf->line)
        {
            start_col = buf->column;
            end_col = buf->marker.col;
        }
        else
        {
            start_col = buf->marker.col;
            end_col = buf->column;
        }
    }
    else
    {
        if(buf->column > buf->marker.col)
        {
            start_col = buf->marker.col;
            end_col = buf->column;
        }
        else if(buf->column < buf->marker.col)
        {
            start_col = buf->column;
            end_col = buf->marker.col;
        }
    }
    
    XstringDestroy(clipboard.text);
    clipboard.text = XstringCreate("");
    
    if(buf->line != buf->marker.row)//multi line
    {
        node *cur = buf->line_node;
        
        if(buf->line > buf->marker.row)
        {
            // Firstly move to the node where the marker is at
            for (int i = 0; i < line_diff; ++i)
            {
                cur = cur->prev;
            }
        }
        
        for (int i = 0; i <= line_diff; ++i)
        {
            int s = 0;
            int e = strlen(cur->data);
            
            if(i == 0)
            {
                s = start_col;
            }
            if(i == line_diff)
            {
                e = end_col;
            }
            
            char *ch = (char*)calloc(2,1);
            
            for (int j = s; j < e; ++j)
            {
                ch[0] = cur->data[j];
                XstringConcat(clipboard.text, 1, ch);
            }
            
            free(ch);
            
            if(i != line_diff)
            {
#ifdef _WIN32
                XstringConcat(clipboard.text, 1, "\r\n");
#endif
#ifdef __linux__
                XstringConcat(clipboard.text, 1, "\n");
#endif
            }
            
            cur = cur->next;
        }
    }
    else//single line
    {
        char *ch = (char*)calloc(2,1);
        
        for (int i = start_col; i < end_col; ++i)
        {
            ch[0] = buf->line_node->data[i];
            XstringConcat(clipboard.text, 1, ch);
        }
        
        free(ch);
    }
    
    SDL_SetClipboardText(XstringGet(clipboard.text));
    clipboard.has_content = true;
}

void ClipBoardCut(buffer *buf)
{
    if(buf->column == buf->marker.col && buf->line == buf->marker.row)
    {
        return;
    }
    
    int start_line = MIN(buf->line, buf->marker.row);
    int end_line = MAX(buf->line, buf->marker.row);
    int line_diff = end_line - start_line;
    int start_col, end_col;
    
    if(buf->line != buf->marker.row)
    {
        if(start_line == buf->line)
        {
            start_col = buf->column;
            end_col = buf->marker.col;
        }
        else
        {
            start_col = buf->marker.col;
            end_col = buf->column;
        }
    }
    else
    {
        if(buf->column > buf->marker.col)
        {
            start_col = buf->marker.col;
            end_col = buf->column;
        }
        else if(buf->column < buf->marker.col)
        {
            start_col = buf->column;
            end_col = buf->marker.col;
        }
    }
    
    XstringDestroy(clipboard.text);
    clipboard.text = XstringCreate("");
    
    if(buf->line != buf->marker.row)//multi line
    {
        xstring *merged = XstringCreate("");
        node *cur = buf->line_node;
        node *ref = cur;
        
        if(buf->line > buf->marker.row)
        {
            // Firstly move to the node where the marker is at
            for (int i = 0; i < line_diff; ++i)
            {
                cur = cur->prev;
            }
            
            ref = cur;
        }
        
        for (int i = 0; i <= line_diff; ++i)
        {
            int s = 0;
            int e = strlen(cur->data);
            
            if(i == 0)
            {
                s = start_col;
                
                // Copy left of start line to merged
                char *ch = (char*)calloc(2,1);
                
                for (int i = 0; i < start_col; ++i)
                {
                    ch[0] = cur->data[i];
                    XstringConcat(merged, 1, ch);
                }
                
                free(ch);
            }
            if(i == line_diff)
            {
                e = end_col;
                
                // Copy right of end line to merged
                int len = strlen(cur->data);
                char *ch = (char*)calloc(2,1);
                
                for (int i = e; i < len; ++i)
                {
                    ch[0] = cur->data[i];
                    XstringConcat(merged, 1, ch);
                }
                
                free(ch);
            }
            
            char *ch = (char*)calloc(2,1);
            
            for (int j = s; j < e; ++j)
            {
                ch[0] = cur->data[j];
                XstringConcat(clipboard.text, 1, ch);
            }
            
            free(ch);
            
            if(i != line_diff)
            {
#ifdef _WIN32
                XstringConcat(clipboard.text, 1, "\r\n");
#endif
#ifdef __linux__
                XstringConcat(clipboard.text, 1, "\n");
#endif
            }
            
            cur = cur->next;
        }
        
        // Delete lines
        for (int i = end_line; i >= start_line ; --i)
        {
            LineDelete(buf, i);
        }
        
        // Insert merged line
        LineInsert(buf, start_line);
        int chunks = (merged->length / settings.line_mem_chunk) + 1;
        
        if(buf->line_node->num_chunks < chunks)
        {
            LineRequestMemChunks(buf->line_node, chunks);
        }
        
        memcpy(buf->line_node->data, merged->data, merged->length);
        XstringDestroy(merged);
        buf->line = start_line;
        buf->column = start_col;
        
        // Ensure scroll offset not exceeded because of the deleted lines
        int max_scroll_offset_ver;
        
        if(buf->line_count > buf->panel.row_capacity)
        {
            max_scroll_offset_ver = buf->line_count - buf->panel.row_capacity;
        }
        else
        {
            max_scroll_offset_ver = 0;
        }
        
        if(buf->panel.scroll_offset_ver > max_scroll_offset_ver)
        {
            buf->panel.scroll_offset_ver = max_scroll_offset_ver;
        }
    }
    else//single line
    {
        int len = strlen(buf->line_node->data);
        xstring *merged = XstringCreate("");
        
        for (int i = 0; i < len; ++i)
        {
            char *ch = (char*)calloc(2,1);
            
            if(i >= start_col && i < end_col)
            {
                ch[0] = buf->line_node->data[i];
                XstringConcat(clipboard.text, 1, ch);
            }
            
            else if(start_col > 0 || end_col < len)//Copy leftovers(if any)
            {
                ch[0] = buf->line_node->data[i];
                XstringConcat(merged, 1, ch);
            }
            
            free(ch);
            buf->line_node->data[i] = 0;
        }
        
        int merged_len = merged->length;
        
        for (int i = 0; i < merged_len; ++i)
        {
            buf->line_node->data[i] = merged->data[i];
        }
        
        XstringDestroy(merged);
        LineShrinkMemChunks(buf->line_node);
        buf->column = start_col;
        buf->marker.col = start_col;
    }
    
    SDL_SetClipboardText(XstringGet(clipboard.text));
    clipboard.has_content = true;
    
    buf->marker.row = buf->line;
    buf->marker.col = buf->column;
    SyncCursorWithBuffer(buf);
    
    RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
}

void ClipBoardPaste(buffer *buf)
{
    if(clipboard.has_content)
    {
        XstringSet(clipboard.text, SDL_GetClipboardText());
        
        int len = XstringGetLength(clipboard.text);
        
        for (int i = 0; i < len; ++i)
        {
            if(clipboard.text->data[i] != '\r' && clipboard.text->data[i] != '\n')
            {
                LineEnsureSufficientCapacity(buf->line_node);
                int len = strlen(buf->line_node->data);
                
                if(buf->column < len)
                {
                    int char_count = len - buf->column;
                    memmove(buf->line_node->data + buf->column + 1, buf->line_node->data + buf->column, char_count);
                }
                
                *(buf->line_node->data + buf->column) = clipboard.text->data[i];
                buf->column++;
            }
            
            if (clipboard.text->data[i] == '\n')//windows & linux??
            {
                //Input_TextEd_Return(buf);
                buf->column = 0;
                buf->line++;
                SyncCursorWithBuffer(buf);
                LineInsert(buf, buf->line);
                
                buf->cursor.last_hor_pos = buf->cursor.col;
                
                //scroll
                if(buf->line_count > buf->panel.row_capacity && buf->cursor.row >= buf->panel.row_capacity)
                {
                    buf->panel.scroll_offset_ver++;
                    RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
                    SyncCursorWithBuffer(buf);
                }
                
                SwitchHorizontalPage(buf);
            }
        }
        
        buf->marker.row = buf->line;
        buf->marker.col = buf->column;
        SyncCursorWithBuffer(buf);
        
        RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
    }
}

void ClipBoardGetExternal()
{
    if(SDL_HasClipboardText())
    {
        char *ext = SDL_GetClipboardText();
        XstringSet(clipboard.text, ext);
        clipboard.has_content = true;
    }
    else
    {
        clipboard.has_content = false;
    }
}
