#include "input.h"
#include "line.h"
#include "character.h"
#include "draw.h"
#include <iostream>

node *InputText(buffer *buf, node *cur_node)
{
    InsertCharacterAt(buf, cur_node, buf->column);
    RenderCharacterAt(buf, cur_node, buf->cursor.row, buf->cursor.col - 1, strlen(cur_node->data), characters_texture, buf->panel.texture);
    buf->cursor.last_hor_pos = buf->cursor.col;
    
    return cur_node;
};


node *InputReturn(buffer *buf, node *cur_node)
{
    if (buf->column == strlen(cur_node->data))//cursor at end of line
    {
        buf->column = 0;
        buf->line++;
        SyncCursorWithBuffer(buf);
        cur_node = InsertLineAt(buf, buf->line);
        memset(cur_node, 0, 256);
        
        // re-draw trailing lines if this isn't the last line entered
        if (buf->line <  buf->line_count - 1)
        {
            RenderClearLine(buf, cur_node, buf->cursor.row, characters_texture, buf->panel.texture);
        }
    }
    else// cursor at start or middle of line
    {
        cur_node = InsertLineAt(buf, buf->line + 1);
        memset(cur_node, 0, 256);
        
        int index = 0;
        int len = strlen(cur_node->prev->data);
        
        for (int i = buf->column; i < len; ++i)
        {
            cur_node->data[index] = cur_node->prev->data[i];
            cur_node->prev->data[i] = '\0';
            index++;
        }
        
        RenderClearLine(buf, cur_node->prev, buf->cursor.row, characters_texture, buf->panel.texture);
        
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
    return cur_node;
};


node *InputBackspace(buffer *buf, node *cur_node)
{
    cur_node = DeleteCharacterAt(buf, cur_node, buf->column);
    RenderClearCharacterAt(buf, cur_node, buf->cursor.row, buf->cursor.col, strlen(cur_node->data),characters_texture, buf->panel.texture);
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
    
    return cur_node;
};


node *InputDelete(buffer *buf, node *cur_node)
{
    if(buf->column < strlen(cur_node->data))
    {
        buf->column++;
        SyncCursorWithBuffer(buf);
        cur_node = DeleteCharacterAt(buf, cur_node, buf->column);
        RenderClearCharacterAt(buf, cur_node, buf->cursor.row, buf->cursor.col, strlen(cur_node->data),characters_texture, buf->panel.texture);
    }
    else if(buf->line < buf->line_count - 1)
    {
        buf->line++;
        buf->column = 0;
        SyncCursorWithBuffer(buf);
        cur_node = cur_node->next;
        cur_node = DeleteCharacterAt(buf, cur_node, buf->column);
        RenderClearCharacterAt(buf, cur_node, buf->cursor.row, buf->cursor.col, strlen(cur_node->data),characters_texture, buf->panel.texture);
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
    
    return cur_node;
};


node *InputLeft(buffer *buf, node *cur_node)
{
    if(buf->column > 0)
    {
        buf->column--;
    }
    else
    {
        if(cur_node->prev != buf->head)
        {
            cur_node = cur_node->prev;
            buf->line--;
            buf->column = strlen(cur_node->data);
        }
    }
    
    SwitchHorizontalPage(buf);
    SyncCursorWithBuffer(buf);
    buf->cursor.last_hor_pos = buf->cursor.col;
    
    return cur_node;
};


node *InputRight(buffer *buf, node *cur_node)
{
    if(buf->column < strlen(cur_node->data))
    {
        buf->column++;
    }
    else
    {
        if(cur_node->next != NULL)
        {
            cur_node = cur_node->next;
            buf->line++;
            buf->column = 0;
        }
    }
    
    SwitchHorizontalPage(buf);
    SyncCursorWithBuffer(buf);
    buf->cursor.last_hor_pos = buf->cursor.col;
    
    return cur_node;
};


node *InputUp(buffer *buf, node *cur_node)
{
    if(buf->line > 0)
    {
        if(buf->cursor.last_hor_pos <= strlen(cur_node->prev->data))
        {
            buf->column = buf->cursor.last_hor_pos;
        }
        else
        {
            buf->column = strlen(cur_node->prev->data);
        }
        
        SwitchHorizontalPage(buf);
        buf->line--;
        SyncCursorWithBuffer(buf);
        cur_node = cur_node->prev;
    }
    else
    {
        buf->column = 0;
        SyncCursorWithBuffer(buf);
    }
    
    //test scroll
    if(buf->line_count > buf->panel.row_capacity && buf->cursor.row < 0)
    {
        buf->panel.scroll_offset_ver--;
        RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
        SyncCursorWithBuffer(buf);
    }
    
    return cur_node;
};


node *InputDown(buffer *buf, node *cur_node)
{
    if(buf->line < buf->line_count - 1)
    {
        if(buf->cursor.last_hor_pos <= strlen(cur_node->next->data))
        {
            buf->column = buf->cursor.last_hor_pos;
        }
        else
        {
            buf->column = strlen(cur_node->next->data);
        }
        
        SwitchHorizontalPage(buf);
        buf->line++;
        SyncCursorWithBuffer(buf);
        cur_node = cur_node->next;
    }
    else
    {
        buf->column = strlen(cur_node->data);
        SyncCursorWithBuffer(buf);
    }
    
    //test scroll
    if(buf->line_count > buf->panel.row_capacity && buf->cursor.row >= buf->panel.row_capacity)
    {
        buf->panel.scroll_offset_ver++;
        RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
        SyncCursorWithBuffer(buf);
    }
    
    return cur_node;
};


void InputTab(buffer *buf, node *cur_node)
{
    for (int i = 0; i < 4; ++i)
    {
        U8_strinsert(cur_node->data, buf->column, " ", 256);
        buf->column++;
        SyncCursorWithBuffer(buf);
        RenderCharacterAt(buf, cur_node, buf->cursor.row, buf->cursor.col - 1, strlen(cur_node->data), characters_texture, buf->panel.texture);
        buf->cursor.last_hor_pos = buf->cursor.col;
    }
};

void InputHome(buffer *buf, node *cur_node)
{
    if(buf->column > 0)
    {
        buf->column = 0;
        buf->cursor.last_hor_pos = 0;
        SwitchHorizontalPage(buf);
        SyncCursorWithBuffer(buf);
    }
};

void InputEnd(buffer *buf, node *cur_node)
{
    int len = strlen(cur_node->data);
    buf->column = len;
    buf->cursor.last_hor_pos = len;
    SwitchHorizontalPage(buf);
    SyncCursorWithBuffer(buf);
};

node *InputPageUp(buffer *buf, node *cur_node)
{
    if(buf->line_count > buf->panel.row_capacity && buf->line > buf->panel.row_capacity)
    {
        for (int i = buf->line; i > buf->line - buf->panel.row_capacity; --i)
        {
            cur_node = cur_node->prev;
        }
        
        buf->line -= buf->panel.row_capacity;
        SwitchHorizontalPage(buf);
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
            SyncCursorWithBuffer(buf);
        }
        
    }
    else
    {
        buf->line = 0;
        buf->panel.scroll_offset_ver = 0;
        SwitchHorizontalPage(buf);
        RenderLineRange(buf, 0, buf->panel.row_capacity, characters_texture, buf->panel.texture);
        SyncCursorWithBuffer(buf);
    }
    
    return cur_node;
};

node *InputPageDown(buffer *buf, node *cur_node)
{
    if(buf->line_count > buf->panel.row_capacity && buf->line < buf->line_count - buf->panel.row_capacity)
    {
        for (int i = buf->line; i < buf->line + buf->panel.row_capacity; ++i)
        {
            cur_node = cur_node->next;
        }
        
        buf->line += buf->panel.row_capacity;
        SwitchHorizontalPage(buf);
        SyncCursorWithBuffer(buf);
        
        //test scroll
        int aaa = (buf->line_count / buf->panel.row_capacity) * buf->panel.row_capacity;
        if(buf->line <=  buf->line_count - (aaa+1))
        {
            buf->panel.scroll_offset_ver += buf->panel.row_capacity;
            
            RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
            SyncCursorWithBuffer(buf);
        }
        else
        {
            //std::cout << "h" << std::endl;
            //buf->panel.scroll_offset_ver += buf->line_count - buf->line - buf->panel.row_capacity;
            //SyncCursorWithBuffer(buf);
            //RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
            //SyncCursorWithBuffer(buf);
        }
        
    }
    else
    {
        buf->line = buf->line_count-1;
        SwitchHorizontalPage(buf);
        RenderLineRange(buf, buf->line_count - buf->panel.row_capacity, buf->panel.row_capacity, characters_texture, buf->panel.texture);
        SyncCursorWithBuffer(buf);
    }
    
    return cur_node;
};