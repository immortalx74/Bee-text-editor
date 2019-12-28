#include "undoredo.h"

void UndoStackStoreOp(buffer *buf, op_type t, int row, int col, char *text)
{
    if(t == OP_INSERT)
    {
        if(UNDOREDO_IDX >= 0)
        {
            undoredo_op last_op = undo_stack[UNDOREDO_IDX];
            int len = XstringGetLength(last_op.text);
            char last_char = XstringGet(last_op.text)[len - 1];
            
            if(!last_op.locked && buf->line == last_op.row 
               && col == last_op.col + len 
               && IsCharacterAlphaNumeric(text[0]) 
               && IsCharacterAlphaNumeric(last_char))// keep adding chars to this OP
            {
                XstringConcat(last_op.text, 1, text);
            }
            else// break previous OP and add a new one
            {
                last_op.locked = true;
                bool l;
                if(IsCharacterAlphaNumeric(text[0]))
                {
                    l = false;
                }
                else
                {
                    l = true;
                }
                xstring *txt = XstringCreate(text);
                undoredo_op op = {buf, t, row, col, l, txt};
                UNDOREDO_INC;
                undo_stack[UNDOREDO_IDX] = op;
                
                if(undoredo_counter < UNDO_STEPS)
                {
                    undoredo_counter++;
                }
            }
        }
        else// first OP added to the stack
        {
            bool l;
            if(IsCharacterAlphaNumeric(text[0]))
            {
                l = false;
            }
            else
            {
                l = true;
            }
            xstring *txt = XstringCreate(text);
            undoredo_op op = {buf, t, row, col, l, txt};
            UNDOREDO_INC;
            undo_stack[UNDOREDO_IDX] = op;
            
            if(undoredo_counter < UNDO_STEPS)
            {
                undoredo_counter++;
            }
        }
    }
    else if(t == OP_DELETE)
    {
        xstring *txt = XstringCreate(text);
        undoredo_op op = {buf, t, row, col, true, txt};
        UNDOREDO_INC;
        undo_stack[UNDOREDO_IDX] = op;
        
        if(undoredo_counter < UNDO_STEPS)
        {
            undoredo_counter++;
        }
    }
};

void UndoStackCommitUndo(buffer *buf)
{
    undoredo_index = UNDOREDOISNEGATIVE();
    if(undoredo_counter == 0)
    {
        return;
    }
    
    undoredo_op last_op = undo_stack[UNDOREDO_IDX];
    node *nd = buf->line_node;
    
    if(last_op.type == OP_INSERT)
    {
        if(buf->line != last_op.row) //move to the correct node
        {
            int diff = MAX(buf->line, last_op.row) - MIN(buf->line, last_op.row);
            
            for (int i = 0; i < diff; ++i)
            {
                if(buf->line < last_op.row)
                {
                    nd = nd->next;
                }
                else if(buf->line > last_op.row)
                {
                    nd = nd->prev;
                }
            }
            
            buf->line_node = nd;
        }
        
        if(last_op.col + XstringGetLength(last_op.text) == strlen(nd->data))// string at end of line
        {
            memset(nd->data + last_op.col, 0, XstringGetLength(last_op.text));
            LineShrinkMemChunks(nd);
        }
        else// string somewhere at middle
        {
            int len = XstringGetLength(last_op.text);
            int chars_left = strlen(nd->data) - last_op.col - len;
            memmove(nd->data + last_op.col, nd->data + last_op.col + len, chars_left);
            memset(nd->data + strlen(nd->data) - len, 0, len);
            LineShrinkMemChunks(nd);
        }
        
        buf->line = last_op.row;
        buf->column = last_op.col;
        UNDOREDO_DEC;
        SyncCursorWithBuffer(buf);
        RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
        
        if(undoredo_counter > 0)
        {
            undoredo_counter--;
        }
        undoredo_index = UNDOREDOISNEGATIVE();
    }
    else if(last_op.type == OP_DELETE)
    {
        if(buf->line != last_op.row) //move to the correct node
        {
            int diff = MAX(buf->line, last_op.row) - MIN(buf->line, last_op.row);
            
            for (int i = 0; i < diff; ++i)
            {
                if(buf->line < last_op.row)
                {
                    nd = nd->next;
                }
                else if(buf->line > last_op.row)
                {
                    nd = nd->prev;
                }
            }
            
            buf->line_node = nd;
        }
        int chars_to_move = strlen(nd->data) - last_op.col + 1;
        memmove(nd->data + last_op.col + 1, nd->data + last_op.col, chars_to_move);
        memset(nd->data + last_op.col, XstringGet(last_op.text)[0], 1);
        
        buf->line = last_op.row;
        buf->column = last_op.col + 1;
        UNDOREDO_DEC;
        SyncCursorWithBuffer(buf);
        RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
        
        if(undoredo_counter > 0)
        {
            undoredo_counter--;
        }
        undoredo_index = UNDOREDOISNEGATIVE();
    }
};

void UndoStackCommitRedo(buffer *buf)
{
    if(undoredo_counter == UNDO_STEPS)
    {
        return;
    }
    UNDOREDO_INC;
    undoredo_op last_op = undo_stack[UNDOREDO_IDX];
    node *nd = buf->line_node;
    
    if(last_op.type == OP_INSERT)
    {
        if(buf->line != last_op.row) //move to the correct node
        {
            int diff = MAX(buf->line, last_op.row) - MIN(buf->line, last_op.row);
            
            for (int i = 0; i < diff; ++i)
            {
                if(buf->line < last_op.row)
                {
                    nd = nd->next;
                }
                else if(buf->line > last_op.row)
                {
                    nd = nd->prev;
                }
            }
            
            buf->line_node = nd;
        }
        
        // Grow line capacity if needed
        int line_len = strlen(nd->data);
        int op_len = XstringGetLength(last_op.text);
        int new_len = line_len + op_len;
        LineExpandMemChunks(nd, new_len);
        
        if(last_op.col  == line_len)// string at end of line
        {
            memcpy(nd->data + last_op.col, XstringGet(last_op.text), op_len);
        }
        else// string somewhere at middle
        {
            int char_count = line_len - last_op.col;
            memmove(nd->data + last_op.col + op_len, nd->data + last_op.col, char_count);
            memcpy(nd->data + last_op.col, XstringGet(last_op.text), op_len);
        }
        
        buf->line = last_op.row;
        buf->column = last_op.col + op_len;
        SyncCursorWithBuffer(buf);
        RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
        
        if(undoredo_counter < UNDO_STEPS)
        {
            undoredo_counter++;
        }
    }
    else if(last_op.type == OP_DELETE)
    {
        if(buf->line != last_op.row) //move to the correct node
        {
            int diff = MAX(buf->line, last_op.row) - MIN(buf->line, last_op.row);
            
            for (int i = 0; i < diff; ++i)
            {
                if(buf->line < last_op.row)
                {
                    nd = nd->next;
                }
                else if(buf->line > last_op.row)
                {
                    nd = nd->prev;
                }
            }
            
            buf->line_node = nd;
        }
        
        int op_len = XstringGetLength(last_op.text);
        int chars_to_move = strlen(nd->data) - op_len;
        memcpy(nd->data + last_op.col, nd->data + last_op.col + 1, chars_to_move);
        
        buf->line = last_op.row;
        buf->column = last_op.col;
        SyncCursorWithBuffer(buf);
        RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
        
        if(undoredo_counter < UNDO_STEPS)
        {
            undoredo_counter++;
        }
    }
};