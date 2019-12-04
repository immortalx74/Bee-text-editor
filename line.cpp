#include "line.h"
#include <iostream>
#include "draw.h"

// Create line and return pointer to it
node *CreateLine(void)
{
    node *newline = (node*)malloc(sizeof(node));
    newline->next = NULL;
    newline->prev = NULL;
    
    return newline;
};

// Insert line at position. Return NULL on error or node pointer on success
node *InsertLineAt(buffer *buf, int pos)
{
    node *head_current = buf->head;
    
    if(pos < 0 || pos > buf->line_count)
    {
        node *p = NULL;
        return p;
    }
    else if(pos == 0) // node at start of list
    {
        node *right = head_current->next;
        node *newline = CreateLine();
        
        if(right == NULL) // No nodes yet
        {
            head_current->next = newline;
            newline->prev = head_current;
        }
        else // Nodes exist but we want the new node at pos = 0
        {
            node *right_of_head = head_current->next;
            head_current->next = newline;
            newline->prev = head_current;
            newline->next = right_of_head;
        }
        
        memset(newline->data, 0, 256);
        
        buf->line_count++;
        return newline;
    }
    else if(pos == buf->line_count && buf->line_count >= 1) // add as last node
    {
        int count = 0;
        node *newline = CreateLine();
        node *ptr = head_current;
        
        while(count < pos)
        {
            ptr = ptr->next;
            count++;
        };
        
        // ptr at this point is the last node (before insertion of the new node)
        newline->prev = ptr;
        ptr->next = newline;
        newline->next = NULL;
        buf->line_count++;
        
        memset(newline->data, 0, 256);
        return newline;
    }
    else // Add in-between(NOTE: if pos = n, pushes existing n and all other nodes rightwards)
    {
        int count = 0;
        node *newline = CreateLine();
        node *ptr = head_current;
        
        while(count <= pos)
        {
            ptr = ptr->next;
            count++;
        };
        
        node *left = ptr->prev;
        node *right = ptr;
        
        left->next = newline;
        right->prev = newline;
        newline->prev = left;
        newline->next = right;
        
        buf->line_count++;
        
        memset(newline->data, 0, 256);
        return newline;
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
        free(line);
        buf->line_count--;
        
        return;
    }
};


node *GetLineNode(buffer *buf, int pos)
{
    node *cur_head = buf->head;
    
    for (int i = 0; i < pos; ++i)
    {
        cur_head = cur_head->next;
    }
    
    return cur_head->next;
};

node *KillBuffer(buffer *buf)
{
    node *n = buf->head->next;
    node *ahead = NULL;
    
    while(n != NULL)
    {
        ahead= n->next;
        free(n);
        n = ahead;
    };
    
    buf->head->prev = NULL;
    buf->head->next = NULL;
    n = InsertLineAt(buf, 0);
    
    buf->line = 0;
    buf->line_count = 1;
    buf->column = 0;
    buf->cursor.row = 0;
    buf->cursor.col = 0;
    buf->cursor.last_hor_pos = 0;
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

void AttemptSetToLastColumn(buffer *buf, node *cur_node)
{
    if(buf->cursor.last_hor_pos <= strlen(cur_node->data))
    {
        buf->column = buf->cursor.last_hor_pos;
    }
    else
    {
        buf->column = strlen(cur_node->data);
    }
};