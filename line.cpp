struct node
{
    char data[200];
    struct node *next;
    struct node *prev;
};

node *head = (node*)malloc(sizeof(node));
int line_count = 0;

// Create line and return pointer to it
node *CreateLine(void)
{
    node *newline = (node*)malloc(sizeof(node));
    newline->next = NULL;
    newline->prev = NULL;
    
    return newline;
};

// Insert line at position. Return NULL on error or node pointer on success
node *InsertLineAt(int pos)
{
    if(pos < 0 || pos > line_count)
    {
        node *p = NULL;
        return p;
    }
    else if(pos == 0) // First node
    {
        node *right = head->next;
        node *newline = CreateLine();
        
        if(right == NULL) // No nodes yet
        {
            head->next = newline;
            newline->prev = head;
        }
        else // Nodes exist but we want the new node at pos = 0
        {
            node *right_of_head = head->next;
            head->next = newline;
            newline->prev = head;
            newline->next = right_of_head;
        }
        
        line_count++;
        return newline;
    }
    else if(pos == line_count && line_count >= 1) // add as last node
    {
        int count = 0;
        node *newline = CreateLine();
        node *ptr = head;
        
        while(count < pos)
        {
            ptr = ptr->next;
            count++;
        };
        
        // ptr at this point is the last node (before insertion of the new node)
        newline->prev = ptr;
        ptr->next = newline;
        newline->next = NULL;
        line_count++;
        return newline;
    }
    else // Add in-between(NOTE: if pos = n, pushes existing n and all other nodes rightwards)
    {
        int count = 0;
        node *newline = CreateLine();
        node *ptr = head;
        
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
        
        line_count++;
        
        return newline;
    }
}

void DeleteLineAt(int pos)
{
    if(pos < 0 || pos > line_count - 1)
    {
        return;
    }
    else if(pos == 0) // Delete first node
    {
        node *line = head->next;
        head->next = head->next->next;
        head->next->next->prev = head;
        free(line);
        
        return;
    }
    else if(pos == line_count - 1) // Delete last node
    {
        node *line = head;
        int count = 0;
        
        while(count <= pos)
        {
            line = line->next;
            count++;
        }
        line->prev->next = NULL;
        free(line);
        
        return;
    }
    else // Delete node in-between
    {
        node *line = head;
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
        
        return;
    }
};