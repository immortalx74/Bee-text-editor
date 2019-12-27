#include "undoredo.h"

void UndoStackStoreOp(buffer *buf, op_type t, int row, int col, char *text)
{
    if(t == OP_INSERT)
    {
        if(undo_rec_index >= 0)
        {
            undoredo_op last_op = undo_stack[undo_rec_index];
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
                undo_rec_index++;
                undo_stack[undo_rec_index] = op;
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
            undo_rec_index++;
            undo_stack[undo_rec_index] = op;
        }
    }
    
    std::cout << undo_stack[undo_rec_index].text->data << std::endl;
};

void UndoStackCommitUndo(buffer *buf)
{
    undoredo_op last_op = undo_stack[undo_rec_index];
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
        
        if(last_op.col + XstringGetLength(last_op.text) == strlen(nd->data))// string at the end of line
        {
            memset(nd->data + last_op.col, 0, XstringGetLength(last_op.text));
        }
        else
        {
            int len = XstringGetLength(last_op.text);
            int chars_left = strlen(nd->data) - last_op.col - len;
            memmove(nd->data + last_op.col, nd->data + last_op.col + len, chars_left);
            memset(nd->data + strlen(nd->data) - len, 0, len);
        }
        
        buf->line = last_op.row;
        buf->column = last_op.col;
        undo_rec_index--;
        SyncCursorWithBuffer(buf);
        RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
    }
};

