//UNDOREDO.H
void UndoStackStoreOp(buffer *buf, op_type t, int row, int col, char *text);
void UndoStackCommitUndo(buffer *buf);
void UndoStackCommitRedo(buffer *buf);

void UndoOpInsert(buffer *buf, undoredo_op op);
void UndoOpDelete(buffer *buf, undoredo_op op);
void RedoOpInsert(buffer *buf, undoredo_op op);
void RedoOpDelete(buffer *buf, undoredo_op op);


// undoredo model:

// set a undoredo_slots_batch variable?
// start with an allocation: 50 states * sizeof(undoredo_op)
// each new action is a pointer offset of this allocation
// so, when a new action is recorded, setup its fields
// if the last action(current???) number equals 50, rea-allocate 50 more

// keep track of last, current action
// last = 10, current = 5
// if current = 5 and a new "DO" command is commited, then current becomes 5 too.
// all undo states after 5 are discarded?

// possible actions:
// 1. insert char stream
// 2. delete char stream
// 3. insert new line
// 4. delete new line
// 5. insert single or multi-line stream (from paste)
// 6. delete single or multi-line stream (from cut or range-delete)

1
2
3


//UNDOREDO.CPP
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
                
                if(undoredo_counter < settings.undo_steps)
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
            
            if(undoredo_counter < settings.undo_steps)
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
        
        if(undoredo_counter < settings.undo_steps)
        {
            undoredo_counter++;
        }
    }
};

void UndoStackCommitUndo(buffer *buf)
{
    if(undoredo_counter == 0)
    {
        return;
    }
    
    undoredo_index = UNDOREDOISNEGATIVE();
    undoredo_op last_op = undo_stack[UNDOREDO_IDX];
    
    if(last_op.type == OP_INSERT)
    {
        UndoOpInsert(buf, last_op);
    }
    else if(last_op.type == OP_DELETE)
    {
        UndoOpDelete(buf, last_op);
    }
};

void UndoStackCommitRedo(buffer *buf)
{
    if(undoredo_counter == settings.undo_steps)
    {
        return;
    }
    UNDOREDO_INC;
    undoredo_op last_op = undo_stack[UNDOREDO_IDX];
    
    if(last_op.type == OP_INSERT)
    {
        RedoOpInsert(buf, last_op);
    }
    else if(last_op.type == OP_DELETE)
    {
        RedoOpDelete(buf, last_op);
    }
};

void UndoOpInsert(buffer *buf, undoredo_op op)
{
    node *nd = buf->line_node;
    
    if(buf->line != op.row) //move to the correct node
    {
        int diff = MAX(buf->line, op.row) - MIN(buf->line, op.row);
        
        for (int i = 0; i < diff; ++i)
        {
            if(buf->line < op.row)
            {
                nd = nd->next;
            }
            else if(buf->line > op.row)
            {
                nd = nd->prev;
            }
        }
        
        buf->line_node = nd;
    }
    
    if(op.col + XstringGetLength(op.text) == strlen(nd->data))// string at end of line
    {
        memset(nd->data + op.col, 0, XstringGetLength(op.text));
        LineShrinkMemChunks(nd);
    }
    else// string somewhere at middle
    {
        int len = XstringGetLength(op.text);
        int chars_left = strlen(nd->data) - op.col - len;
        memmove(nd->data + op.col, nd->data + op.col + len, chars_left);
        memset(nd->data + strlen(nd->data) - len, 0, len);
        LineShrinkMemChunks(nd);
    }
    
    buf->line = op.row;
    buf->column = op.col;
    UNDOREDO_DEC;
    SyncCursorWithBuffer(buf);
    RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
    
    if(undoredo_counter > 0)
    {
        undoredo_counter--;
    }
    undoredo_index = UNDOREDOISNEGATIVE();
};

void UndoOpDelete(buffer *buf, undoredo_op op)
{
    node *nd = buf->line_node;
    
    if(buf->line != op.row) //move to the correct node
    {
        int diff = MAX(buf->line, op.row) - MIN(buf->line, op.row);
        
        for (int i = 0; i < diff; ++i)
        {
            if(buf->line < op.row)
            {
                nd = nd->next;
            }
            else if(buf->line > op.row)
            {
                nd = nd->prev;
            }
        }
        
        buf->line_node = nd;
    }
    int chars_to_move = strlen(nd->data) - op.col + 1;
    memmove(nd->data + op.col + 1, nd->data + op.col, chars_to_move);
    memset(nd->data + op.col, XstringGet(op.text)[0], 1);
    
    buf->line = op.row;
    buf->column = op.col + 1;
    UNDOREDO_DEC;
    SyncCursorWithBuffer(buf);
    RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
    
    if(undoredo_counter > 0)
    {
        undoredo_counter--;
    }
    undoredo_index = UNDOREDOISNEGATIVE();
};

void RedoOpInsert(buffer *buf, undoredo_op op)
{
    node *nd = buf->line_node;
    
    if(buf->line != op.row) //move to the correct node
    {
        int diff = MAX(buf->line, op.row) - MIN(buf->line, op.row);
        
        for (int i = 0; i < diff; ++i)
        {
            if(buf->line < op.row)
            {
                nd = nd->next;
            }
            else if(buf->line > op.row)
            {
                nd = nd->prev;
            }
        }
        
        buf->line_node = nd;
    }
    
    // Grow line capacity if needed
    int line_len = strlen(nd->data);
    int op_len = XstringGetLength(op.text);
    int new_len = line_len + op_len;
    LineExpandMemChunks(nd, new_len);
    
    if(op.col  == line_len)// string at end of line
    {
        memcpy(nd->data + op.col, XstringGet(op.text), op_len);
    }
    else// string somewhere at middle
    {
        int char_count = line_len - op.col;
        memmove(nd->data + op.col + op_len, nd->data + op.col, char_count);
        memcpy(nd->data + op.col, XstringGet(op.text), op_len);
    }
    
    buf->line = op.row;
    buf->column = op.col + op_len;
    SyncCursorWithBuffer(buf);
    RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
    
    if(undoredo_counter < settings.undo_steps)
    {
        undoredo_counter++;
    }
};


void RedoOpDelete(buffer *buf, undoredo_op op)
{
    node *nd = buf->line_node;
    
    if(buf->line != op.row) //move to the correct node
    {
        int diff = MAX(buf->line, op.row) - MIN(buf->line, op.row);
        
        for (int i = 0; i < diff; ++i)
        {
            if(buf->line < op.row)
            {
                nd = nd->next;
            }
            else if(buf->line > op.row)
            {
                nd = nd->prev;
            }
        }
        
        buf->line_node = nd;
    }
    
    int op_len = XstringGetLength(op.text);
    int chars_to_move = strlen(nd->data) - op_len;
    memcpy(nd->data + op.col, nd->data + op.col + 1, chars_to_move);
    
    buf->line = op.row;
    buf->column = op.col;
    SyncCursorWithBuffer(buf);
    RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
    
    if(undoredo_counter < settings.undo_steps)
    {
        undoredo_counter++;
    }
};void UndoStackStoreOp(buffer *buf, op_type t, int row, int col, char *text)
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
                
                if(undoredo_counter < settings.undo_steps)
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
            
            if(undoredo_counter < settings.undo_steps)
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
        
        if(undoredo_counter < settings.undo_steps)
        {
            undoredo_counter++;
        }
    }
};

void UndoStackCommitUndo(buffer *buf)
{
    if(undoredo_counter == 0)
    {
        return;
    }
    
    undoredo_index = UNDOREDOISNEGATIVE();
    undoredo_op last_op = undo_stack[UNDOREDO_IDX];
    
    if(last_op.type == OP_INSERT)
    {
        UndoOpInsert(buf, last_op);
    }
    else if(last_op.type == OP_DELETE)
    {
        UndoOpDelete(buf, last_op);
    }
};

void UndoStackCommitRedo(buffer *buf)
{
    if(undoredo_counter == settings.undo_steps)
    {
        return;
    }
    UNDOREDO_INC;
    undoredo_op last_op = undo_stack[UNDOREDO_IDX];
    
    if(last_op.type == OP_INSERT)
    {
        RedoOpInsert(buf, last_op);
    }
    else if(last_op.type == OP_DELETE)
    {
        RedoOpDelete(buf, last_op);
    }
};

void UndoOpInsert(buffer *buf, undoredo_op op)
{
    node *nd = buf->line_node;
    
    if(buf->line != op.row) //move to the correct node
    {
        int diff = MAX(buf->line, op.row) - MIN(buf->line, op.row);
        
        for (int i = 0; i < diff; ++i)
        {
            if(buf->line < op.row)
            {
                nd = nd->next;
            }
            else if(buf->line > op.row)
            {
                nd = nd->prev;
            }
        }
        
        buf->line_node = nd;
    }
    
    if(op.col + XstringGetLength(op.text) == strlen(nd->data))// string at end of line
    {
        memset(nd->data + op.col, 0, XstringGetLength(op.text));
        LineShrinkMemChunks(nd);
    }
    else// string somewhere at middle
    {
        int len = XstringGetLength(op.text);
        int chars_left = strlen(nd->data) - op.col - len;
        memmove(nd->data + op.col, nd->data + op.col + len, chars_left);
        memset(nd->data + strlen(nd->data) - len, 0, len);
        LineShrinkMemChunks(nd);
    }
    
    buf->line = op.row;
    buf->column = op.col;
    UNDOREDO_DEC;
    SyncCursorWithBuffer(buf);
    RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
    
    if(undoredo_counter > 0)
    {
        undoredo_counter--;
    }
    undoredo_index = UNDOREDOISNEGATIVE();
};

void UndoOpDelete(buffer *buf, undoredo_op op)
{
    node *nd = buf->line_node;
    
    if(buf->line != op.row) //move to the correct node
    {
        int diff = MAX(buf->line, op.row) - MIN(buf->line, op.row);
        
        for (int i = 0; i < diff; ++i)
        {
            if(buf->line < op.row)
            {
                nd = nd->next;
            }
            else if(buf->line > op.row)
            {
                nd = nd->prev;
            }
        }
        
        buf->line_node = nd;
    }
    int chars_to_move = strlen(nd->data) - op.col + 1;
    memmove(nd->data + op.col + 1, nd->data + op.col, chars_to_move);
    memset(nd->data + op.col, XstringGet(op.text)[0], 1);
    
    buf->line = op.row;
    buf->column = op.col + 1;
    UNDOREDO_DEC;
    SyncCursorWithBuffer(buf);
    RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
    
    if(undoredo_counter > 0)
    {
        undoredo_counter--;
    }
    undoredo_index = UNDOREDOISNEGATIVE();
};

void RedoOpInsert(buffer *buf, undoredo_op op)
{
    node *nd = buf->line_node;
    
    if(buf->line != op.row) //move to the correct node
    {
        int diff = MAX(buf->line, op.row) - MIN(buf->line, op.row);
        
        for (int i = 0; i < diff; ++i)
        {
            if(buf->line < op.row)
            {
                nd = nd->next;
            }
            else if(buf->line > op.row)
            {
                nd = nd->prev;
            }
        }
        
        buf->line_node = nd;
    }
    
    // Grow line capacity if needed
    int line_len = strlen(nd->data);
    int op_len = XstringGetLength(op.text);
    int new_len = line_len + op_len;
    LineExpandMemChunks(nd, new_len);
    
    if(op.col  == line_len)// string at end of line
    {
        memcpy(nd->data + op.col, XstringGet(op.text), op_len);
    }
    else// string somewhere at middle
    {
        int char_count = line_len - op.col;
        memmove(nd->data + op.col + op_len, nd->data + op.col, char_count);
        memcpy(nd->data + op.col, XstringGet(op.text), op_len);
    }
    
    buf->line = op.row;
    buf->column = op.col + op_len;
    SyncCursorWithBuffer(buf);
    RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
    
    if(undoredo_counter < settings.undo_steps)
    {
        undoredo_counter++;
    }
};


void RedoOpDelete(buffer *buf, undoredo_op op)
{
    node *nd = buf->line_node;
    
    if(buf->line != op.row) //move to the correct node
    {
        int diff = MAX(buf->line, op.row) - MIN(buf->line, op.row);
        
        for (int i = 0; i < diff; ++i)
        {
            if(buf->line < op.row)
            {
                nd = nd->next;
            }
            else if(buf->line > op.row)
            {
                nd = nd->prev;
            }
        }
        
        buf->line_node = nd;
    }
    
    int op_len = XstringGetLength(op.text);
    int chars_to_move = strlen(nd->data) - op_len;
    memcpy(nd->data + op.col, nd->data + op.col + 1, chars_to_move);
    
    buf->line = op.row;
    buf->column = op.col;
    SyncCursorWithBuffer(buf);
    RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
    
    if(undoredo_counter < settings.undo_steps)
    {
        undoredo_counter++;
    }
};


12345
51234
45123
34512
23451


1->2->3->4->5

2->3->4->5->1

3->4->5->1->2

0000000000
123456789

H
0

H
0 0

H
0 0 0

H
0 0 0 0

H       T
0 0 0 0 0

T H . . .
0 0 0 0 0

. T H . .
0 0 0 0 0

. . T H .
0 0 0 0 0

.   . T H
0 0 0 0 0

H   . . T
0 0 0 0 0
