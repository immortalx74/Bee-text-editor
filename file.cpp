#include "file.h"
#include <iostream>
#include "draw.h"

void FileReadToBuffer(buffer *buf, char *filename)
{
    std::ifstream file;
    file.open(filename);
    std::string str;
    str.reserve(LINE_MEM_CHUNK);
    
    node *cur_node = KillBuffer(buf);
    
    while (std::getline(file, str))
    {
        if(str.length() / LINE_MEM_CHUNK > 0)
        {
            int num = (str.length() / LINE_MEM_CHUNK) + 1;
            LineRequestMemChunks(cur_node, num);
        }
        
        strcpy(cur_node->data, str.c_str());
        buf->column = strlen(cur_node->data);
        
        node *newline = (node*)malloc(sizeof(node));
        newline->data = (char*)calloc(LINE_MEM_CHUNK, 1);
        newline->num_chunks = 1;
        newline->prev = cur_node;
        newline->next = NULL;
        cur_node->next = newline;
        cur_node = newline;
        buf->line_count++;
    };
    
    //TEMP hack. Delete that extra last line
    DeleteLineAt(buf, buf->line_count - 1);
    
    file.close();
    
    //Set cursor to first line
    cur_node = buf->head->next;
    
    buf->line = 0;
    buf->column = 0;
    buf->cursor.row = 0;
    buf->cursor.col = 0;
    buf->cursor.last_hor_pos = 0;
    buf->panel.scroll_offset_ver = 0;
    buf->panel.page = 0;
    buf->line_node = cur_node;
    
    
    //TEMP set filename
    int len = strlen(buf->filename);
    for (int i = 0; i < len; ++i)
    {
        buf->filename[i] = 0;
    }
    
    len = strlen(filename);
    for (int i = 0; i < len; ++i)
    {
        buf->filename[i] = filename[i];
    }
    
    // Render content
    RenderLineRange(buf, 0, buf->line_count, characters_texture, buf->panel.texture);
};

void FileWriteToDisk(buffer *buf, char *filename)
{
    std::ofstream file;
    file.open(filename);
    
    node *cur = buf->head->next;
    
    while(cur != NULL)
    {
        if (!file)
        {
            std::cout << "Somthing failed while opening the file\n";
            SDL_Delay(1000);
        }
        file << cur->data;
        file << "\n";
        cur = cur->next;
    };
    
    file.close();
};

