#include "input.h"
#include "line.h"
#include "character.h"
#include "draw.h"
#include <iostream>

node *InputText(buffer *buf, node *cur_node)
{
    InsertCharacterAt(buf, cur_node, buf->cursor.column);
    RenderCharacterAt(buf, cur_node, buf->cursor.line, buf->cursor.column - 1, strlen(cur_node->data), characters_texture, app.active_texture);
    buf->cursor.last_hor_pos = buf->cursor.column;
    
    return cur_node;
};


node *InputReturn(buffer *buf, node *cur_node)
{
    if (buf->cursor.column == strlen(cur_node->data))//cursor at end of line
    {
        buf->cursor.column = 0;
        buf->cursor.line++;
        cur_node = InsertLineAt(buf, buf->cursor.line);
        memset(cur_node, 0, 128);
        
        // re-draw trailing lines if this isn't the last line entered
        if (buf->cursor.line <  buf->line_count - 1)
        {
            RenderClearLine(buf, cur_node, buf->cursor.line, characters_texture, app.active_texture);
        }
    }
    else// cursor at start or middle of line
    {
        cur_node = InsertLineAt(buf, buf->cursor.line + 1);
        memset(cur_node, 0, 128);
        
        int index = 0;
        int len = strlen(cur_node->prev->data);
        
        for (int i = buf->cursor.column; i < len; ++i)
        {
            cur_node->data[index] = cur_node->prev->data[i];
            cur_node->prev->data[i] = '\0';
            index++;
        }
        
        RenderClearLine(buf, cur_node->prev, buf->cursor.line, characters_texture, app.active_texture);
        
        buf->cursor.line++;
        buf->cursor.column = 0;
    }
    
    buf->cursor.last_hor_pos = buf->cursor.column;
    
    return cur_node;
};


node *InputBackspace(buffer *buf, node *cur_node)
{
    cur_node= DeleteCharacterAt(buf, cur_node, buf->cursor.column);
    RenderClearCharacterAt(buf, cur_node, buf->cursor.line, buf->cursor.column, strlen(cur_node->data),characters_texture, app.active_texture);
    buf->cursor.last_hor_pos = buf->cursor.column;
    
    return cur_node;
};


node *InputDelete(buffer *buf, node *cur_node)
{
    if(buf->cursor.column < strlen(cur_node->data))
    {
        buf->cursor.column++;
        cur_node = DeleteCharacterAt(buf, cur_node, buf->cursor.column);
        RenderClearCharacterAt(buf, cur_node, buf->cursor.line, buf->cursor.column, strlen(cur_node->data),characters_texture, app.active_texture);
    }
    else if(buf->cursor.line < buf->line_count - 1)
    {
        buf->cursor.line++;
        buf->cursor.column = 0;
        cur_node = cur_node->next;
        cur_node = DeleteCharacterAt(buf, cur_node, buf->cursor.column);
        RenderClearCharacterAt(buf, cur_node, buf->cursor.line, buf->cursor.column, strlen(cur_node->data),characters_texture, app.active_texture);
    }
    buf->cursor.last_hor_pos = buf->cursor.column;
    
    return cur_node;
};


node *InputLeft(buffer *buf, node *cur_node)
{
    if(buf->cursor.column > 0)
    {
        buf->cursor.column--;
    }
    else
    {
        if(cur_node->prev != buf->head)
        {
            cur_node = cur_node->prev;
            buf->cursor.line--;
            buf->cursor.column = strlen(cur_node->data);
        }
    }
    
    buf->cursor.last_hor_pos = buf->cursor.column;
    
    return cur_node;
};


node *InputRight(buffer *buf, node *cur_node)
{
    if(buf->cursor.column < strlen(cur_node->data))
    {
        buf->cursor.column++;
    }
    else
    {
        if(cur_node->next != NULL)
        {
            cur_node = cur_node->next;
            buf->cursor.line++;
            buf->cursor.column = 0;
        }
    }
    
    buf->cursor.last_hor_pos = buf->cursor.column;
    
    return cur_node;
};


node *InputUp(buffer *buf, node *cur_node)
{
    if(buf->cursor.line > 0)
    {
        if(buf->cursor.last_hor_pos <= strlen(cur_node->prev->data))
        {
            buf->cursor.column = buf->cursor.last_hor_pos;
        }
        else
        {
            buf->cursor.column = strlen(cur_node->prev->data);
        }
        
        buf->cursor.line--;
        cur_node = cur_node->prev;
    }
    else
    {
        buf->cursor.column = 0;
    }
    
    return cur_node;
};


node *InputDown(buffer *buf, node *cur_node)
{
    if(buf->cursor.line < buf->line_count - 1)
    {
        if(buf->cursor.last_hor_pos <= strlen(cur_node->next->data))
        {
            buf->cursor.column = buf->cursor.last_hor_pos;
        }
        else
        {
            buf->cursor.column = strlen(cur_node->next->data);
        }
        
        buf->cursor.line++;
        cur_node = cur_node->next;
    }
    else
    {
        buf->cursor.column = strlen(cur_node->data);
    }
    
    return cur_node;
};


void InputTab(buffer *buf, node *cur_node)
{
    for (int i = 0; i < 4; ++i)
    {
        U8_strinsert(cur_node->data, buf->cursor.column, " ", 128);
        buf->cursor.column++;
        RenderCharacterAt(buf, cur_node, buf->cursor.line, buf->cursor.column - 1, strlen(cur_node->data), characters_texture, app.active_texture);
        buf->cursor.last_hor_pos = buf->cursor.column;
    }
};