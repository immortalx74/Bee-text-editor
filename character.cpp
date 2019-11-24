#include "character.h"

void InsertCharacterAt(buffer *buf, node *row, int col)
{
    U8_strinsert(row->data, buf->cursor.column, app.e.text.text, 128);
    buf->cursor.column++;
};

node *DeleteCharacterAt(buffer *buf, node *row, int col)
{
    if(buf->cursor.column > 0)
    {
        buf->cursor.column--;
        U8_strdel(row->data, buf->cursor.column);
        return row;
    }
    else // reached the start of line. merge line with previous and force a re-draw
    {
        if(row->prev != buf->head)
        {
            buf->cursor.column = strlen(row->prev->data);
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
            
            DeleteLineAt(buf, buf->cursor.line);
            row = row->prev;
            buf->cursor.line--;
            
            
            RenderClearLine(buf, row, buf->cursor.line, characters_texture, app.active_texture);
            return row;
        }
        else
        {
            return buf->head->next;
        }
    }
};