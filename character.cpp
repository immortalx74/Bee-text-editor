#include "character.h"
#include <iostream>

void InsertCharacterAt(buffer *buf, node *row, int col)
{
    U8_strinsert(row->data, buf->column, app.e.text.text, 128);
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