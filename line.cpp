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
        
        memset(buf->line_node->data, 0, 256);
        
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
        
        memset(buf->line_node->data, 0, 256);
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
        
        memset(buf->line_node->data, 0, 256);
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
    node *nd = buf->head;
    
    for (int i = 0; i < pos; ++i)
    {
        nd = nd->next;
    }
    
    return nd->next;
};

node *KillBuffer(buffer *buf)
{
    node *n = buf->head->next;
    node *ahead = NULL;
    
    while(n != NULL)
    {
        ahead = n->next;
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