#include "clipboard.h"

void ClipBoardCopy(buffer *buf)
{
    if(buf->column == buf->marker.col && buf->line == buf->marker.row)
    {
        if(strlen(buf->line_node->data + buf->column) == 0)
        {
            return;
        }
    }
    
    int start_line = MIN(buf->line, buf->marker.row);
    int end_line = MAX(buf->line, buf->marker.row);
    int line_diff = end_line - start_line;
    
    XstringDestroy(clipboard.text);
    clipboard.text = XstringCreate("");
    
    int trim_left, trim_right;
    
    if (start_line == buf->line)
    {
        trim_left = buf->column;
        node *temp = buf->line_node;
        
        for (int i = 0; i < line_diff; ++i)
        {
            temp = temp->next;
        }
        
        trim_right = strlen(temp->data) - buf->marker.col;
    }
    else
    {
        trim_left = buf->marker.col;
        trim_right = strlen(buf->line_node->data) - buf->column;
    }
    
    if(buf->line != buf->marker.row)//multi line
    {
        node *cur = buf->line_node;
        
        if(buf->line > buf->marker.row)
        {
            // Firstly move up to the node where the marker is at
            for (int i = 0; i < line_diff; ++i)
            {
                cur = cur->prev;
            }
        }
        
        for (int i = 0; i <= line_diff; ++i)
        {
            if(i == line_diff)
            {
                XstringConcat(clipboard.text, 1, cur->data);
            }
            else
            {
#ifdef _WIN32
                XstringConcat(clipboard.text, 2, cur->data, "\r\n");
#endif
#ifdef __linux__
                XstringConcat(clipboard.text, 2, cur->data, "\n");
#endif
            }
            
            cur = cur->next;
        }
        
        XstringTruncateHead(clipboard.text, trim_left);
        XstringTruncateTail(clipboard.text, trim_right);
    }
    else if(buf->line == buf->marker.row)//single line
    {
        if(buf->column != buf->marker.col)
        {
            node *cur = buf->line_node;
            XstringSet(clipboard.text, cur->data);
            
            int col_left = MIN(buf->column, buf->marker.col);
            int col_right = MAX(buf->column, buf->marker.col);
            col_right= strlen(cur->data) - col_right;
            XstringTruncateHead(clipboard.text, col_left);
            XstringTruncateTail(clipboard.text, col_right);
        }
        else//copy single character
        {
            char *singlechar = (char*)calloc(2, 1);
            memcpy(singlechar, buf->line_node->data + buf->column, 1);
            XstringSet(clipboard.text, singlechar);
            free(singlechar);
        }
    }
    
    SDL_SetClipboardText(XstringGet(clipboard.text));
};

void ClipBoardCut(buffer *buf)
{
    if(buf->column == buf->marker.col && buf->line == buf->marker.row)
    {
        if(strlen(buf->line_node->data + buf->column) == 0)
        {
            return;
        }
    }
    
    int start_line = MIN(buf->line, buf->marker.row);
    int end_line = MAX(buf->line, buf->marker.row);
    int line_diff = end_line - start_line;
    
    XstringDestroy(clipboard.text);
    clipboard.text = XstringCreate("");
    
    int trim_left, trim_right;
    
    if (start_line == buf->line)
    {
        trim_left = buf->column;
        node *temp = buf->line_node;
        
        for (int i = 0; i < line_diff; ++i)
        {
            temp = temp->next;
        }
        
        trim_right = strlen(temp->data) - buf->marker.col;
    }
    else
    {
        trim_left = buf->marker.col;
        trim_right = strlen(buf->line_node->data) - buf->column;
    }
    
    if(buf->line != buf->marker.row)//multi line
    {
        node *cur = buf->line_node;
        
        if(buf->line > buf->marker.row)
        {
            // Firstly move to the node where the marker is at
            for (int i = 0; i < line_diff; ++i)
            {
                cur = cur->prev;
                buf->line--;
            }
            buf->line_node = cur;
            
        }
        
        for (int i = 0; i <= line_diff; ++i)
        {
            if(i < line_diff)
            {
#ifdef _WIN32
                XstringConcat(clipboard.text, 2, cur->data, "\r\n");
#endif
#ifdef __linux__
                XstringConcat(clipboard.text, 2, cur->data, "\n");
#endif
                if(i == 0 && trim_left > 0)// don't delete entire line, just the chars AFTER left
                {
                    int chars_to_zero = strlen(cur->data) - trim_left;
                    memset(cur->data + trim_left, 0, chars_to_zero);
                    cur = cur->next;
                }
                else if(i == 0 && trim_left == 0)// still don't delete line. Just delete all chars
                {
                    memset(cur->data, 0, strlen(cur->data));
                    cur = cur->next;
                }
                else if(i > 0)
                {
                    cur = cur->next;
                    DeleteLineAt(buf, buf->line + i);
                }
            }
            else if(i == line_diff)
            {
                XstringConcat(clipboard.text, 1, cur->data);
                
                if(trim_right == 0)// delete entire last line
                {
                    DeleteLineAt(buf, buf->line + 1);
                }
                else// Copy the chars AFTER right and merge with previous line 
                {
                    int total_new_len = strlen(cur->prev->data) + trim_right;
                    int prev_capacity_left = (cur->prev->num_chunks * settings.line_mem_chunk) - strlen(cur->prev->data);
                    if(prev_capacity_left < trim_right)
                    {
                        LineRequestMemChunks(cur->prev, ((prev_capacity_left + trim_right) / settings.line_mem_chunk) + 1);
                    }
                    
                    memcpy(cur->prev->data + strlen(cur->prev->data), cur->data + strlen(cur->data) - trim_right, trim_right);
                    DeleteLineAt(buf, buf->line + i);
                }
            }
        }
        
        XstringTruncateHead(clipboard.text, trim_left);
        XstringTruncateTail(clipboard.text, trim_right);
        buf->column = trim_left;
        SyncCursorWithBuffer(buf);
    }
    else if(buf->line == buf->marker.row)//single line
    {
        if(buf->column != buf->marker.col)
        {
            node *cur = buf->line_node;
            XstringSet(clipboard.text, cur->data);
            
            int col_left = MIN(buf->column, buf->marker.col);
            int col_right = MAX(buf->column, buf->marker.col);
            
            int chars_to_zero = col_right - col_left;
            int col_right_before = col_right;
            
            col_right = strlen(cur->data) - col_right;
            XstringTruncateHead(clipboard.text, col_left);
            XstringTruncateTail(clipboard.text, col_right);
            
            
            int len = strlen(cur->data);
            memmove(cur->data + col_left, cur->data + col_right_before, len - col_right_before);
            memset(cur->data + len - chars_to_zero, 0, chars_to_zero);
            
            buf->column = col_left;
            SyncCursorWithBuffer(buf);
        }
        else//copy single character
        {
            char *singlechar = (char*)calloc(2, 1);
            memcpy(singlechar, buf->line_node->data + buf->column, 1);
            XstringSet(clipboard.text, singlechar);
            free(singlechar);
        }
    }
    
    SDL_SetClipboardText(XstringGet(clipboard.text));
    
    buf->marker.row = buf->line;
    buf->marker.col = buf->column;
    SyncCursorWithBuffer(buf);
    
    RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
};

void ClipBoardPaste(buffer *buf)
{
    if(clipboard.has_content)
    {
        XstringSet(clipboard.text, SDL_GetClipboardText());
        
        int len = XstringGetLength(clipboard.text);
        
        for (int i = 0; i < len; ++i)
        {
            if(XstringGet(clipboard.text)[i] != '\r' && XstringGet(clipboard.text)[i] != '\n')
            {
                LineEnsureSufficientCapacity(buf->line_node);
                
                if(buf->column < strlen(buf->line_node->data))
                {
                    int char_count = strlen(buf->line_node->data) - buf->column;
                    memmove(buf->line_node->data + buf->column + 1, buf->line_node->data + buf->column, char_count);
                }
                
                *(buf->line_node->data + buf->column) = XstringGet(clipboard.text)[i];
                buf->column++;
            }
            
            if (XstringGet(clipboard.text)[i] == '\n')//windows & linux??
            {
                Input_TextEd_Return(buf);
            }
        }
        
        buf->marker.row = buf->line;
        buf->marker.col = buf->column;
        SyncCursorWithBuffer(buf);
        
        RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
    }
};

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
};

