#include "file.h"

node *FileReadToBuffer(buffer *buf, char *filename)
{
    std::ifstream file;
    file.open(filename);
    std::string str;
    
    //TEMP:Discard previous contents of buffer
    buf->line = 0;
    buf->line_count = 0;
    buf->column = 0;
    buf->cursor.row = 0;
    buf->cursor.col = 0;
    buf->panel.scroll_offset_ver = 0;
    node *cur_node = KillBuffer(buf);
    
    //while (std::getline(file, str))
    //{
    //strcpy(cur_node->data, str.c_str());
    //buf->column = strlen(cur_node->data);
    //cur_node= InputReturn(buf, cur_node);
    //};
    
    file.close();
    
    //Set cursor to first line
    cur_node = buf->head->next;
    buf->line = 0;
    buf->column = 0;
    buf->cursor.row = 0;
    buf->cursor.col = 0;
    buf->panel.scroll_offset_ver = 0;
    
    return cur_node;
};