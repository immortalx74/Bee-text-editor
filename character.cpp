#include "character.h"
#include <iostream>

void InsertCharacterAt(buffer *buf, node *row, int col)
{
    U8_strinsert(row->data, buf->column, app.e.text.text, 256);
    buf->column++;
    SyncCursorWithBuffer(buf);
};

node *DeleteCharacterAt(buffer *buf, node *row, int col)
{
    if(buf->column > 0)
    {
        buf->column--;
        SyncCursorWithBuffer(buf);
        U8_strdel(row->data, buf->column);
        return row;
    }
    else // reached the start of line. merge line with previous and force a re-draw
    {
        if(row->prev != buf->head)
        {
            buf->column = strlen(row->prev->data);
            
            // check if there are characters left in line
            if(strlen(row->data) > 0)
            {
                int pos = strlen(row->prev->data);
                
                for (int i = 0; i < strlen(row->data); ++i)
                {
                    row->prev->data[pos] = row->data[i];
                    pos++;
                }
            }
            
            DeleteLineAt(buf, buf->line);
            row = row->prev;
            buf->line--;
            SyncCursorWithBuffer(buf);
            
            RenderClearLine(buf, row, buf->line, characters_texture, buf->panel.texture);
            
            return row;
        }
        else
        {
            return buf->head->next;
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