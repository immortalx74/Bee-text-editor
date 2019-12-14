#include "list.h"
#include <iostream>

list *ListCreate(char title_text[], int cap, int el_size)
{
    list *l = (list*)malloc(sizeof(list));
    
    l->filter = (char*)malloc(20);
    memset(l->filter, 0, 20);
    
    l->title = (char*)malloc(strlen(title_text) + 1);
    l->title[strlen(l->title)] = 0;
    strcpy(l->title, title_text);
    
    l->capacity = cap;
    l->element_size = el_size;
    l->data = (char*)malloc(cap * el_size);
    l->selected = 0;
    l->row = 0;
    l->scroll_offset = 0;
    l->current_path = app.last_path;
    memset(l->data, 0, cap * el_size);
    return l;
};

void ListDelete(list *l)
{
    free(l->data);
    free(l->title);
    free(l->filter);
    free(l);
};

void ListClear(list *l)
{
    free(l->data);
    
    l->data = (char*)malloc(l->capacity * l->element_size);
    l->selected = 0;
    l->row = 0;
    l->scroll_offset = 0;
    memset(l->data, 0, l->capacity * l->element_size);
};

void ListResize(list *l, int new_cap)
{
    (char*)realloc(l->data, new_cap * l->element_size);
    l->capacity = new_cap;
};

void ListSetElement(list *l, int index, char text[])
{
    char *elem = l->data + (index * l->element_size);
    memset(elem, 0, l->element_size);
    memcpy(elem, text, l->element_size);
    elem[l->element_size - 1] = '\0';
};

char *ListGetElement(list *l, int index)
{
    char *elem = l->data + (index * l->element_size);
    return elem;
};

void PopulateFileList(list *l, char *path)
{
    tinydir_dir dir;
    tinydir_open(&dir, path);
    int count = 0;
    int skip = 0;
    l->element_count= 0;
    l->current_path = (char*)malloc(strlen(path) + 1);
    l->current_path[strlen(l->current_path) - 1] = 0;
    strcpy(l->current_path, path);
    char *current;
    
    
    while (dir.has_next)
    {
        if(skip > 1)
        {
            tinydir_file file;
            tinydir_readfile(&dir, &file);
            
            current = file.name;
            
            if (file.is_dir)
            {
                int len = strlen(current);
                current[len] = '\\';
                current[len + 1] = '\0';
            }
            
            ListSetElement(l, count, current);
            l->element_count++;
            
            count++;
        }
        
        tinydir_next(&dir);
        skip++;
    }
    
    tinydir_close(&dir);
};

void FilterFileList(list *l, char *path)
{
    tinydir_dir dir;
    tinydir_open(&dir, path);
    int count = 0;
    l->element_count= 0;
    l->current_path = (char*)malloc(strlen(path));
    strcpy(l->current_path, path);
    char *current;
    
    while (dir.has_next)
    {
        tinydir_file file;
        tinydir_readfile(&dir, &file);
        
        current = file.name;
        
        if(strstr(current, l->filter) != NULL)
        {
            if (file.is_dir)
            {
                int len = strlen(current);
                current[len] = '\\';
                current[len + 1] = '\0';
            }
            
            ListSetElement(l, count, current);
            l->element_count++;
            count++;
        }
        
        tinydir_next(&dir);
    }
    
    tinydir_close(&dir);
};

void ListSwitchToSelectedDirectory(list *l, char *sel_dir)
{
    int new_size = strlen(l->current_path) + strlen(sel_dir) + 1;
    char *new_path = (char*)calloc(new_size, 1);
    
    
    strcpy(new_path, l->current_path);
    strcat(new_path, sel_dir);
    
    l->current_path = new_path;
    
    ListClear(l);
    memset(l->filter, 0, sizeof(l->filter));
    PopulateFileList(l, new_path);
    
    free(new_path);
};

void ListLoadSelectedFile(list *l, char *sel_file)
{
    app.last_path = l->current_path;
    int size = strlen(l->current_path) + strlen(sel_file) + 1;
    char *path_and_filename = (char*)calloc(size, 1);
    
    strcpy(path_and_filename, l->current_path);
    strcat(path_and_filename, sel_file);
    
    FileReadToBuffer(app.active_buffer, path_and_filename);
    
    free(path_and_filename);
    
    if(app.active_buffer->lst != NULL)
    {
        ListDelete(app.active_buffer->lst);
        app.active_buffer->lst = NULL;
        app.mode = TEXT_EDIT;
        RenderLineRange(app.active_buffer, app.active_buffer->panel.scroll_offset_ver, app.active_buffer->panel.row_capacity, characters_texture, app.active_buffer->panel.texture);
    }
};

//list *GenerateFileList(buffer *buf)
//{
//list *flist = ListCreate("Open:", 100, buf->panel.col_capacity - 5);
//
//tinydir_dir dir;
//tinydir_open(&dir, "e:/dev/ed");
//int count = 0;
//
//while (dir.has_next)
//{
//tinydir_file file;
//tinydir_readfile(&dir, &file);
//
//ListSetElement(flist, count, file.name);
//
//tinydir_next(&dir);
//count++;
//}
//
//tinydir_close(&dir);
//
//return flist;
//};