0123456789

oooooooooo
1111111111


















//undostackstoreop
if(t == OP_INSERT)
{
    if(undo_rec_index >= 0)
    {
        int last_char_index = strlen(undo_stack[undo_rec_index].text->data) - 1;
        int len = XstringGetLength(undo_stack[undo_rec_index].text);
        
        if(!undo_stack[undo_rec_index - 1].locked && col ==  len && buf->line == row && IsCharacterAlphaNumeric(text[0]) && IsCharacterAlphaNumeric(undo_stack[undo_rec_index].text->data[last_char_index]))// keep adding chars to this OP
        {
            XstringConcat(undo_stack[undo_rec_index].text, 1, text);
        }
        else// break previous OP and add a new one
        {
            undo_stack[undo_rec_index].locked = true;
            
            xstring *txt = XstringCreate(text);
            text_op op = {buf, t, row, col, false, txt};
            
            undo_stack[undo_rec_index].col++;
            undo_rec_index++;
            undo_stack[undo_rec_index] = op;
        }
    }
    else// first OP added to the stack
    {
        xstring *txt = XstringCreate(text);
        text_op op = {buf, t, row, col, false, txt};
        
        undo_stack[undo_rec_index].col++;
        undo_rec_index++;
        undo_stack[undo_rec_index] = op;
    }
}

qwerty123zxcvbnm

qwerty,u
qwertyuiop123456
qwp12345600000000
//clipboard
else if( app.e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL)
{
    int start_line = MIN(app.active_buffer->line, app.active_buffer->marker.row);
    int end_line = MAX(app.active_buffer->line, app.active_buffer->marker.row);
    int line_diff = end_line - start_line;
    
    XstringDestroy(clipboard.text);
    clipboard.text = XstringCreate("");
    
    clipboard.line_lengths = (int*)realloc(clipboard.line_lengths, (line_diff + 1) * sizeof(int));
    
    int trim_left, trim_right;
    
    if (start_line == app.active_buffer->line)
    {
        trim_left = app.active_buffer->column;
        node *temp = app.active_buffer->line_node;
        
        for (int i = 0; i < line_diff; ++i)
        {
            temp = temp->next;
        }
        
        trim_right = strlen(temp->data) - app.active_buffer->marker.col;
    }
    else
    {
        trim_left = app.active_buffer->marker.col;
        trim_right = strlen(app.active_buffer->line_node->data) - app.active_buffer->column;
    }
    
    if(app.active_buffer->line < app.active_buffer->marker.row)//cursor first
    {
        node *cur = app.active_buffer->line_node;
        
        for (int i = 0; i <= line_diff; ++i)
        {
            clipboard.line_lengths[i] = strlen(cur->data);
            
            if(i == 0)
            {
                clipboard.line_lengths[i] -= trim_left;
            }
            if(i == line_diff)
            {
                clipboard.line_lengths[i] -= trim_right;
            }
            
            XstringConcat(clipboard.text, 2, XstringGet(clipboard.text), cur->data);
            cur = cur->next;
        }
        
        XstringTruncateHead(clipboard.text, trim_left);
        XstringTruncateTail(clipboard.text, trim_right);
    }
    else if(app.active_buffer->line > app.active_buffer->marker.row)//marker first
    {
        node *cur = app.active_buffer->line_node;
        
        // Firstly move to the node where the marker is at
        for (int i = 0; i < line_diff; ++i)
        {
            cur = cur->prev;
        }
        
        for (int i = 0; i <= line_diff; ++i)
        {
            clipboard.line_lengths[i] = strlen(cur->data);
            
            if(i == 0)
            {
                clipboard.line_lengths[i] -= trim_left;
            }
            if(i == line_diff)
            {
                clipboard.line_lengths[i] -= trim_right;
            }
            
            XstringConcat(clipboard.text, 2, XstringGet(clipboard.text), cur->data);
            cur = cur->next;
        }
        
        XstringTruncateHead(clipboard.text, trim_left);
        XstringTruncateTail(clipboard.text, trim_right);
    }
    else if(app.active_buffer->line == app.active_buffer->marker.row)//cursor row = marker row
    {
        if(app.active_buffer->column != app.active_buffer->marker.col)
        {
            node *cur = app.active_buffer->line_node;
            XstringSet(clipboard.text, cur->data);
            
            int col_left = MIN(app.active_buffer->column, app.active_buffer->marker.col);
            int col_right = MAX(app.active_buffer->column, app.active_buffer->marker.col);
            col_right= strlen(cur->data) - col_right;
            XstringTruncateHead(clipboard.text, col_left);
            XstringTruncateTail(clipboard.text, col_right);
        }
        else//copy single character
        {
            char *singlechar = (char*)calloc(2, 1);
            memcpy(singlechar, app.active_buffer->line_node->data + app.active_buffer->column, 1);
            XstringSet(clipboard.text, singlechar);
            free(singlechar);
        }
    }
}


//concat
void XstringConcat(xstring *str, int arg_count, ...)
{
    va_list valist;
    int total_length = 0;
    int index = 0;
    int *sizes = (int*)malloc(arg_count * sizeof(int));
    
    va_start(valist, arg_count);
    for (int i = 0; i < arg_count; ++i)
    {
        sizes[i] = strlen(va_arg(valist, char*));
        total_length += sizes[i];
    }
    va_end(valist);
    
    str->length = total_length;
    str->data = (char*)realloc(str->data, total_length + 1);
    
    va_start(valist, arg_count);
    
    int offset = 0;
    for (int i = 0; i < arg_count; ++i)
    {
        //memmove(str->data + offset, va_arg(valist, char*), sizes[i]);
        //offset += sizes[i];
        
        if(i == 0)
        {
            //just a call to va_arg in order to move to next argument
            char *dummy = va_arg(valist, char*);
            offset += sizes[i];
        }
        else
        {
            memmove(str->data + offset, va_arg(valist, char*), sizes[i]);
            offset += sizes[i];
        }
    }
    va_end(valist);
    
    free(sizes);
    
    str->data[str->length] = 0;
};


// PASTE
void ClipBoardPaste()
{
    if(clipboard.has_content)
    {
        XstringSet(clipboard.text, SDL_GetClipboardText());
        
        int len = XstringGetLength(clipboard.text);
        
        for (int i = 0; i < len; ++i)
        {
            if(XstringGet(clipboard.text)[i] != '\r' || XstringGet(clipboard.text)[i] != '\n')
            {
                LineEnsureSufficientCapacity(app.active_buffer->line_node);
                
                if(app.active_buffer->column < strlen(app.active_buffer->line_node->data))
                {
                    int char_count = strlen(app.active_buffer->line_node->data) - app.active_buffer->column;
                    memmove(app.active_buffer->line_node->data + app.active_buffer->column + 1, app.active_buffer->line_node->data + app.active_buffer->column, char_count);
                }
                
                *(app.active_buffer->line_node->data + app.active_buffer->column) = XstringGet(clipboard.text)[i];
                app.active_buffer->column++;
            }
            if (XstringGet(clipboard.text)[i] == '\n')//windows test
            {
                Input_TextEd_Return(app.active_buffer);
                //app.active_buffer->column = strlen(app.active_buffer->line_node->data);
            }
        }
        
        RenderLineRange(app.active_buffer, app.active_buffer->panel.scroll_offset_ver, app.active_buffer->panel.row_capacity, characters_texture, app.active_buffer->panel.texture);
    }
};