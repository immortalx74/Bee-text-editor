#include "line.h"
#include <iostream>
#include "draw.h"

void LineEnsureSufficientCapacity(node *ln)
{
    if(strlen(ln->data) + 1 >= ln->num_chunks * LINE_MEM_CHUNK)
    {
        LineRequestMemChunks(ln, 1);
    }
};

void LineRequestMemChunks(node *ln, int num)
{
    int old_num_chunks = ln->num_chunks;
    ln->num_chunks += num;
    int new_size = ln->num_chunks * LINE_MEM_CHUNK;
    ln->data = (char*)realloc(ln->data, new_size);
    memset(ln->data + (old_num_chunks * LINE_MEM_CHUNK), 0, num * LINE_MEM_CHUNK);
};

void LineShrinkMemChunks(node *ln)
{
    int len = strlen(ln->data);
    ln->num_chunks = (len / LINE_MEM_CHUNK) + 1;
    int new_size = ln->num_chunks * LINE_MEM_CHUNK;
    ln->data = (char*)realloc(ln->data, new_size);
    memset(ln->data + len, 0, new_size - len);
};

// Create line and return pointer to it
node *CreateLine(void)
{
    node *newline = (node*)malloc(sizeof(node));
    newline->data = (char*)calloc(LINE_MEM_CHUNK, 1);
    newline->num_chunks = 1;
    newline->next = NULL;
    newline->prev = NULL;
    
    return newline;
};

// Insert line at position. Return NULL on error or node pointer on success
void InsertLineAt(buffer *buf, int pos)
{
    if(pos < 0 || pos > buf->line_count)
    {
        node *p = NULL;
        return;
    }
    else if(pos == 0) // node at start of list
    {
        node *right = buf->head->next;
        buf->line_node = CreateLine();
        
        if(right == NULL) // No nodes yet
        {
            buf->head->next = buf->line_node;
            buf->line_node->prev = buf->head;
        }
        else // Nodes exist but we want the new node at pos = 0
        {
            node *right_of_head = buf->head->next;
            buf->head->next = buf->line_node;
            buf->line_node->prev = buf->head;
            buf->line_node->next = right_of_head;
        }
        
        //memset(buf->line_node->data, 0, LINE_MEM_CHUNK);
        
        buf->line_count++;
    }
    else if(pos == buf->line_count && buf->line_count >= 1) // add as last node
    {
        int count = 0;
        buf->line_node = CreateLine();
        node *ptr = buf->head;
        
        while(count < pos)
        {
            ptr = ptr->next;
            count++;
        };
        
        // ptr at this point is the last node (before insertion of the new node)
        buf->line_node->prev = ptr;
        ptr->next = buf->line_node;
        buf->line_node->next = NULL;
        buf->line_count++;
        
        //memset(buf->line_node->data, 0, LINE_MEM_CHUNK);
    }
    else // Add in-between(NOTE: if pos = n, pushes existing n and all other nodes rightwards)
    {
        int count = 0;
        buf->line_node = CreateLine();
        node *ptr = buf->head;
        
        while(count <= pos)
        {
            ptr = ptr->next;
            count++;
        };
        
        node *left = ptr->prev;
        node *right = ptr;
        
        left->next = buf->line_node;
        right->prev = buf->line_node;
        buf->line_node->prev = left;
        buf->line_node->next = right;
        
        buf->line_count++;
        
        //memset(buf->line_node->data, 0, LINE_MEM_CHUNK);
    }
};

void DeleteLineAt(buffer *buf, int pos)
{
    node *head_current = buf->head;
    
    
    if(pos < 0 || pos > buf->line_count - 1)
    {
        return;
    }
    else if(pos == 0) // Delete first node
    {
        node *line = head_current->next;
        head_current->next = head_current->next->next;
        head_current->next->next->prev = head_current;
        free(line->data);
        free(line);
        buf->line_count--;
        
        return;
    }
    else if(pos == buf->line_count - 1) // Delete last node
    {
        node *line = head_current;
        int count = 0;
        
        while(count <= pos)
        {
            line = line->next;
            count++;
        }
        line->prev->next = NULL;
        free(line->data);
        free(line);
        buf->line_count--;
        
        return;
    }
    else // Delete node in-between
    {
        node *line = head_current;
        int count = 0;
        
        while(count <= pos)
        {
            line = line->next;
            count++;
        }
        
        node *left = line->prev;
        node *right = line->next;
        
        left->next = right;
        right->prev = left;
        free(line->data);
        free(line);
        buf->line_count--;
        
        return;
    }
};


node *GetLineNode(buffer *buf, int pos)
{
    node *nd = buf->head;
    
    for (int i = 0; i < pos; ++i)
    {
        nd = nd->next;
    }
    
    return nd->next;
};

node *KillBuffer(buffer *buf)
{
    //TEST: should i free head data?
    //free(buf->head->data);
    
    node *n = buf->head->next;
    node *ahead = NULL;
    
    while(n != NULL)
    {
        ahead = n->next;
        free(n->data);
        free(n);
        n = ahead;
    };
    
    buf->head->prev = NULL;
    buf->head->next = NULL;
    InsertLineAt(buf, 0);
    n = buf->line_node; 
    
    buf->line = 0;
    buf->line_count = 1;
    buf->column = 0;
    buf->cursor.row = 0;
    buf->cursor.col = 0;
    buf->cursor.last_hor_pos = 0;
    buf->marker.col = 0;
    buf->marker.row = 0;
    buf->panel.scroll_offset_ver = 0;
    buf->panel.page = 0;
    
    //TEMP set filename
    int len = strlen(buf->filename);
    for (int i = 0; i < len; ++i)
    {
        buf->filename[i] = 0;
    }
    
    char empty_filename[] = "*no file*";
    len = strlen(empty_filename);
    for (int i = 0; i < len; ++i)
    {
        buf->filename[i] = empty_filename[i];
    }
    
    RenderLineRange(buf, 0, 1, characters_texture, buf->panel.texture);
    
    return n;
};

void AttemptSetToLastColumn(buffer *buf)
{
    if(buf->cursor.last_hor_pos <= strlen(buf->line_node->data))
    {
        buf->column = buf->cursor.last_hor_pos;
    }
    else
    {
        buf->column = strlen(buf->line_node->data);
    }
};

void ClipBoardCopy(buffer *buf)
{
    if(buf->column == buf->marker.col && buf->line == buf->marker.row)
    {
        if(strlen(buf->line_node->data + buf->column) == 0)
        {
            return;
        }
    }
    
    int start_line = MIN(buf->line, buf->marker.row);
    int end_line = MAX(buf->line, buf->marker.row);
    int line_diff = end_line - start_line;
    
    XstringDestroy(clipboard.text);
    clipboard.text = XstringCreate("");
    
    int trim_left, trim_right;
    
    if (start_line == buf->line)
    {
        trim_left = buf->column;
        node *temp = buf->line_node;
        
        for (int i = 0; i < line_diff; ++i)
        {
            temp = temp->next;
        }
        
        trim_right = strlen(temp->data) - buf->marker.col;
    }
    else
    {
        trim_left = buf->marker.col;
        trim_right = strlen(buf->line_node->data) - buf->column;
    }
    
    if(buf->line != buf->marker.row)//multi line
    {
        node *cur = buf->line_node;
        
        if(buf->line > buf->marker.row)
        {
            // Firstly move up to the node where the marker is at
            for (int i = 0; i < line_diff; ++i)
            {
                cur = cur->prev;
            }
        }
        
        for (int i = 0; i <= line_diff; ++i)
        {
            if(i == line_diff)
            {
                XstringConcat(clipboard.text, 1, cur->data);
            }
            else
            {
#ifdef _WIN32
                XstringConcat(clipboard.text, 2, cur->data, "\r\n");
#endif
#ifdef __linux__
                XstringConcat(clipboard.text, 2, cur->data, "\n");
#endif
            }
            
            cur = cur->next;
        }
        
        XstringTruncateHead(clipboard.text, trim_left);
        XstringTruncateTail(clipboard.text, trim_right);
    }
    else if(buf->line == buf->marker.row)//single line
    {
        if(buf->column != buf->marker.col)
        {
            node *cur = buf->line_node;
            XstringSet(clipboard.text, cur->data);
            
            int col_left = MIN(buf->column, buf->marker.col);
            int col_right = MAX(buf->column, buf->marker.col);
            col_right= strlen(cur->data) - col_right;
            XstringTruncateHead(clipboard.text, col_left);
            XstringTruncateTail(clipboard.text, col_right);
        }
        else//copy single character
        {
            char *singlechar = (char*)calloc(2, 1);
            memcpy(singlechar, buf->line_node->data + buf->column, 1);
            XstringSet(clipboard.text, singlechar);
            free(singlechar);
        }
    }
    
    SDL_SetClipboardText(XstringGet(clipboard.text));
};

void ClipBoardCut(buffer *buf)
{
    if(buf->column == buf->marker.col && buf->line == buf->marker.row)
    {
        if(strlen(buf->line_node->data + buf->column) == 0)
        {
            return;
        }
    }
    
    int start_line = MIN(buf->line, buf->marker.row);
    int end_line = MAX(buf->line, buf->marker.row);
    int line_diff = end_line - start_line;
    
    XstringDestroy(clipboard.text);
    clipboard.text = XstringCreate("");
    
    int trim_left, trim_right;
    
    if (start_line == buf->line)
    {
        trim_left = buf->column;
        node *temp = buf->line_node;
        
        for (int i = 0; i < line_diff; ++i)
        {
            temp = temp->next;
        }
        
        trim_right = strlen(temp->data) - buf->marker.col;
    }
    else
    {
        trim_left = buf->marker.col;
        trim_right = strlen(buf->line_node->data) - buf->column;
    }
    
    if(buf->line != buf->marker.row)//multi line
    {
        node *cur = buf->line_node;
        
        if(buf->line > buf->marker.row)
        {
            // Firstly move to the node where the marker is at
            for (int i = 0; i < line_diff; ++i)
            {
                cur = cur->prev;
                buf->line--;
            }
            buf->line_node = cur;
        }
        
        for (int i = 0; i <= line_diff; ++i)
        {
            if(i == line_diff)
            {
                XstringConcat(clipboard.text, 1, cur->data);
                
                if(trim_right == 0)
                {
                    node *ln = buf->line_node->prev;
                    DeleteLineAt(buf, buf->line);
                    buf->line_node = ln;
                    
                    if(buf->line_node->next != NULL)
                    {
                        buf->line_node = buf->line_node->next;
                    }
                }
                else// don't delete entire line, just some chars from right 
                {
                    node *ln = buf->line_node->prev;
                    int new_len = strlen(cur->data) - trim_right;
                    memset(cur->data + new_len, 0, trim_right);
                    //buf->line_node = ln;
                    buf->line_node = buf->head->next;
                }
            }
            else
            {
#ifdef _WIN32
                XstringConcat(clipboard.text, 2, cur->data, "\r\n");
#endif
#ifdef __linux__
                XstringConcat(clipboard.text, 2, cur->data, "\n");
#endif
                cur = cur->next;
                
                if(i == 0 && trim_left > 0)// don't delete entire line, just some chars from left
                {
                    int new_len = strlen(cur->prev->data) - trim_left;
                    memmove(cur->prev->data, cur->prev->data + trim_left, new_len);
                    memset(cur->prev->data + new_len, 0, trim_left);
                    buf->line_node = cur;
                }
                else if(i >= 0)
                {
                    DeleteLineAt(buf, buf->line);
                    buf->line_node = cur;
                }
            }
        }
        
        XstringTruncateHead(clipboard.text, trim_left);
        XstringTruncateTail(clipboard.text, trim_right);
        std::cout << clipboard.text->data << std::endl;
    }
    else if(buf->line == buf->marker.row)//single line
    {
        if(buf->column != buf->marker.col)
        {
            node *cur = buf->line_node;
            XstringSet(clipboard.text, cur->data);
            
            int col_left = MIN(buf->column, buf->marker.col);
            int col_right = MAX(buf->column, buf->marker.col);
            col_right= strlen(cur->data) - col_right;
            XstringTruncateHead(clipboard.text, col_left);
            XstringTruncateTail(clipboard.text, col_right);
        }
        else//copy single character
        {
            char *singlechar = (char*)calloc(2, 1);
            memcpy(singlechar, buf->line_node->data + buf->column, 1);
            XstringSet(clipboard.text, singlechar);
            free(singlechar);
        }
    }
    
    SDL_SetClipboardText(XstringGet(clipboard.text));
    
    buf->marker.row = buf->line;
    buf->marker.col = buf->column;
    SyncCursorWithBuffer(buf);
    
    RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
};

void ClipBoardPaste(buffer *buf)
{
    if(clipboard.has_content)
    {
        XstringSet(clipboard.text, SDL_GetClipboardText());
        
        int len = XstringGetLength(clipboard.text);
        
        for (int i = 0; i < len; ++i)
        {
            if(XstringGet(clipboard.text)[i] != '\r' && XstringGet(clipboard.text)[i] != '\n')
            {
                LineEnsureSufficientCapacity(buf->line_node);
                
                if(buf->column < strlen(buf->line_node->data))
                {
                    int char_count = strlen(buf->line_node->data) - buf->column;
                    memmove(buf->line_node->data + buf->column + 1, buf->line_node->data + buf->column, char_count);
                }
                
                *(buf->line_node->data + buf->column) = XstringGet(clipboard.text)[i];
                buf->column++;
            }
            
            if (XstringGet(clipboard.text)[i] == '\n')//windows & linux??
            {
                Input_TextEd_Return(buf);
            }
        }
        
        buf->marker.row = buf->line;
        buf->marker.col = buf->column;
        SyncCursorWithBuffer(buf);
        
        RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
    }
};

void ClipBoardGetExternal()
{
    if(SDL_HasClipboardText())
    {
        char *ext = SDL_GetClipboardText();
        XstringSet(clipboard.text, ext);
        clipboard.has_content = true;
    }
    else
    {
        clipboard.has_content = false;
    }
};