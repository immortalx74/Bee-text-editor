#include "undoredo.h"

void UndoStackStoreOp(buffer *buf, op_type t, int row, int col, char *text)
{
    if(t == INSERT)
    {
        if(undo_stack[undo_rec_index].col == col - 1 && IsCharacterAlphaNumeric(text[0]) && undo_rec_index >= 0)
        {
            if(IsCharacterAlphaNumeric(undo_stack[undo_rec_index].text->data[strlen(undo_stack[undo_rec_index].text->data) - 1]))
            {
                XstringConcat(undo_stack[undo_rec_index].text, 1, text);
                undo_stack[undo_rec_index].col++;
                std::cout << undo_stack[undo_rec_index].text->data << std::endl;
                std::cout << undo_rec_index << std::endl;
            }
            
            //return;
        }
        else if(undo_stack[undo_rec_index].col != col - 1 || !IsCharacterAlphaNumeric(text[0]))
        {
            xstring *txt = XstringCreate(text);
            text_op op = {buf, t, row, col, txt};
            
            undo_rec_index++;
            undo_stack[undo_rec_index] = op;
            std::cout << undo_stack[undo_rec_index].text->data << std::endl;
            std::cout << undo_rec_index << std::endl;
            //return;
        }
        else
        {
            xstring *txt = XstringCreate(text);
            text_op op = {buf, t, row, col, txt};
            
            undo_rec_index++;
            undo_stack[undo_rec_index] = op;
            std::cout << undo_stack[undo_rec_index].text->data << std::endl;
            std::cout << undo_rec_index << std::endl;
            //return;
        }
        
    }
    
    
};

