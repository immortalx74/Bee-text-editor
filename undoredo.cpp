#include "undoredo.h"

void UndoRedo_AllocateBatch(buffer *buf)
{
    buf->undoredo.slots = (undoredo_op*)realloc(buf->undoredo.slots, buf->undoredo.batch_count * sizeof(undoredo_op));
    
    xstring *txt = XstringCreate("");
    buf->undoredo.slots[buf->undoredo.current].text = txt;
}

void UndoRedo_StoreOp(buffer *buf, undoredo_op_type t, int row, int col, char *text)
{
    if(t == OP_INSERT)
    {
        undoredo_op last_op = buf->undoredo.slots[buf->undoredo.current];
        int len = XstringGetLength(last_op.text);
        char last_char = XstringGet(last_op.text)[len - 1];
        
        if(last_op.type == OP_INSERT
           && !last_op.locked
           && buf->line == last_op.row
           && col == last_op.col + len 
           && IsCharacterAlphaNumeric(text[0]) 
           && IsCharacterAlphaNumeric(last_char)) // keep adding chars
        {
            XstringConcat(last_op.text , 1, text);
            buf->undoredo.slots[buf->undoredo.current] = last_op;
        }
        else // lock last op and add a new one
        {
            last_op.locked = true;
            xstring *txt = XstringCreate(text);
            undoredo_op op = {t, row, col, false, txt};
            buf->undoredo.current++;
            buf->undoredo.slots[buf->undoredo.current] = op;
        }
        
        std::cout << buf->undoredo.slots[buf->undoredo.current].text->data << std::endl;
    }
}




















//undoredo_op *UndoRedo_CreateOp(void)
//{
//undoredo_op *new_op = (undoredo_op*)malloc(sizeof(undoredo_op));
//xstring *txt = XstringCreate("");
//new_op->text = txt;
//new_op->next = NULL;
//
//if(undoredo_monitor.op_counter == 0)// first op
//{
//new_op->prev = NULL;
//undoredo_monitor.head_op = new_op;
//}
//else if(undoredo_monitor.op_counter == settings.undo_steps)// last op
//{
//undoredo_monitor.tail_op = new_op;
//}
//else
//{
//undoredo_monitor.current_op->next = new_op;
//new_op->prev = undoredo_monitor.current_op;
//}
//
//undoredo_monitor.op_counter++;
//undoredo_monitor.current_op = new_op;
//
//return new_op;
//};
//
//undoredo_op *UndoRedo_GetNextSlot(void)
//{
//if(undoredo_monitor.op_counter < settings.undo_steps)
//{
//return UndoRedo_CreateOp();
//}
//else
//{
//roll head and tail
//undoredo_op *old_head = undoredo_monitor.head_op;
//undoredo_monitor.head_op = undoredo_monitor.head_op->next;
//undoredo_monitor.tail_op = old_head;
//
//return old_head;
//}
//};
//
//void UndoRedo_StoreOp(buffer *buf, undoredo_op_type t, int row, int col, char *text)
//{
//undoredo_op *op;
//
//
//if(t == OP_INSERT)
//{
//
//}
//};