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
    else if(pos == 0) // First node
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
        
        return newline;
    }
}

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