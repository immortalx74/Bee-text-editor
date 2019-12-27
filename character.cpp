#include "character.h"
#include <iostream>

void InsertCharacterAt(buffer *buf, int col)
{
    LineEnsureSufficientCapacity(buf->line_node);
    
    if(col < strlen(buf->line_node->data))
    {
        int char_count = strlen(buf->line_node->data) - col;
        memmove(buf->line_node->data + col + 1, buf->line_node->data + col, char_count);
    }
    
    *(buf->line_node->data + buf->column) = app.e.text.text[0];
    
    UndoStackStoreOp(buf, OP_INSERT, buf->line, buf->column, app.e.text.text);
    buf->column++;
    SyncCursorWithBuffer(buf);
};

void DeleteCharacterAt(buffer *buf, int col)
{
    if(buf->column > 0)
    {
        buf->column--;
        
        //update marker pos
        if(buf->marker.row == buf->line)
        {
            if(buf->column < buf->marker.col)
            {
                buf->marker.col--;
            }
        }
        
        SyncCursorWithBuffer(buf);
        U8_strdel(buf->line_node->data, buf->column);
    }
    else // reached the start of line. merge line with previous and force a re-draw
    {
        if(buf->line_node->prev != buf->head)
        {
            buf->column = strlen(buf->line_node->prev->data);
            
            // update marker pos
            if(buf->marker.row == buf->line)
            {
                buf->marker.row--;
                buf->marker.col += buf->column;
            }
            else if(buf->marker.row > buf->line)
            {
                buf->marker.row--;
            }
            
            // check if there are characters left in line
            if(strlen(buf->line_node->data) > 0)
            {
                int pos = strlen(buf->line_node->prev->data);
                
                int prev_capacity_left = (buf->line_node->prev->num_chunks * LINE_MEM_CHUNK) - strlen(buf->line_node->prev->data);
                int cur_char_count = strlen(buf->line_node->data);
                
                if(cur_char_count > prev_capacity_left)
                {
                    int chunks_to_ask = ((cur_char_count - prev_capacity_left) / LINE_MEM_CHUNK) + 1;
                    LineRequestMemChunks(buf->line_node->prev, chunks_to_ask);
                }
                
                for (int i = 0; i < strlen(buf->line_node->data); ++i)
                {
                    buf->line_node->prev->data[pos] = buf->line_node->data[i];
                    pos++;
                }
            }
            
            DeleteLineAt(buf, buf->line);
            buf->line_node = buf->line_node->prev;
            buf->line--;
            SyncCursorWithBuffer(buf);
            
            RenderClearLine(buf, buf->line, characters_texture, buf->panel.texture);
        }
        else
        {
            buf->line_node = buf->head->next;
        }
    }
};

int U8_strinsert(char* string, int position, const char* source, int max_bytes)
{
    int pos_u8char;
    int len;
    int add_len;
    int ulen;
    const char* string_start = string;
    
    if(string == NULL || source == NULL)
        return 0;
    
    len = strlen(string);
    add_len = strlen(source);
    ulen = U8_strlen(string);
    
    if(position == -1)
        position = ulen;
    
    if(position < 0 || position > ulen || len + add_len + 1 > max_bytes)
        return 0;
    
    // Move string pointer to the proper position
    pos_u8char = 0;
    while(*string != '\0' && pos_u8char < position)
    {
        string = (char*)U8_next(string);
        ++pos_u8char;
    }
    
    // Move the rest of the string out of the way
    memmove(string + add_len, string, len - (string - string_start) + 1);
    
    // Copy in the new characters
    memcpy(string, source, add_len);
    
    return 1;
};

void U8_strdel(char* string, int position)
{
    if(string == NULL || position < 0)
        return;
    
    while(*string != '\0')
    {
        if(position == 0)
        {
            int chars_to_erase = U8_charsize(string);
            int remaining_bytes = strlen(string) + 1;
            memmove(string, string + chars_to_erase, remaining_bytes);
            break;
        }
        
        string = (char*)U8_next(string);
        --position;
    }
};

int U8_strlen(const char* string)
{
    int length = 0;
    if(string == NULL)
        return 0;
    
    while(*string != '\0')
    {
        string = U8_next(string);
        ++length;
    }
    
    return length;
};

const char* U8_next(const char* string)
{
    return string + U8_charsize(string);
};

int U8_charsize(const char* character)
{
    if(character == NULL)
        return 0;
    
    if((unsigned char)*character <= 0x7F)
        return 1;
    else if((unsigned char)*character < 0xE0)
        return 2;
    else if((unsigned char)*character < 0xF0)
        return 3;
    else
        return 4;
    return 1;
};