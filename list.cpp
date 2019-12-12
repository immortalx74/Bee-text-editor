#include "list.h"
#include <iostream>

list *ListCreate(char title_text[], int cap, int el_size)
{
    list *l = (list*)malloc(sizeof(list));
    
    memset(l->title, 0, 256);
    strcpy(l->title, title_text);
    
    l->capacity = cap;
    l->element_size = el_size;
    l->data = (char*)malloc(cap * el_size);
    l->selected = 0;
    l->row = 0;
    l->scroll_offset = 0;
    memset(l->data, 0, cap * el_size);
    return l;
};

void ListDelete(list *l)
{
    free(l->data);
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
    l->element_count= 0;
    l->current_path = (char*)malloc(strlen(path));
    strcpy(l->current_path, path);
    char *current;
    
    
    while (dir.has_next)
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
        
        ListSetElement(l, count, file.name);
        l->element_count++;
        
        tinydir_next(&dir);
        count++;
    }
    
    tinydir_close(&dir);
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