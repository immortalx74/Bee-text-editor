#include "character.h"

void CharacterInsert(buffer *buf, int col)
{
    LineEnsureSufficientCapacity(buf->line_node);
    
    if(col < strlen(buf->line_node->data))
    {
        int char_count = strlen(buf->line_node->data) - col;
        memmove(buf->line_node->data + col + 1, buf->line_node->data + col, char_count);
    }
    
    *(buf->line_node->data + buf->column) = app.e.text.text[0];
    
    UndoRedo_StoreOp(buf, OP_INSERT, buf->line, buf->column, app.e.text.text);
    
    buf->column++;
    SyncCursorWithBuffer(buf);
}

void CharacterDelete(buffer *buf, int col)
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
        
        if(buf->line_node->data == NULL || buf->column < 0)
        {
            return;
        }
        
        int terminator_index = strlen(buf->line_node->data);
        int char_count = terminator_index - buf->column;
        memmove(buf->line_node->data + buf->column, buf->line_node->data + buf->column + 1, char_count);
        buf->line_node->data[terminator_index] = 0;
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
                
                int prev_capacity_left = (buf->line_node->prev->num_chunks * settings.line_mem_chunk) - strlen(buf->line_node->prev->data);
                int cur_char_count = strlen(buf->line_node->data);
                
                if(cur_char_count > prev_capacity_left)
                {
                    int chunks_to_ask = ((cur_char_count - prev_capacity_left) / settings.line_mem_chunk) + 1;
                    LineRequestMemChunks(buf->line_node->prev, chunks_to_ask);
                }
                
                for (int i = 0; i < strlen(buf->line_node->data); ++i)
                {
                    buf->line_node->prev->data[pos] = buf->line_node->data[i];
                    pos++;
                }
            }
            
            LineDelete(buf, buf->line);
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
}