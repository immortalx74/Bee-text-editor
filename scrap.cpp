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