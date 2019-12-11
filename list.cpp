#include "list.h"

list *ListCreate(char title_text[], int cap, int el_size)
{
    list *l = (list*)malloc(sizeof(list));
    
    memset(l->title, 0, 256);
    strcpy(l->title, title_text);
    
    l->capacity = cap;
    l->element_size = el_size;
    l->data = (char*)malloc(cap * el_size);
    l->selected = 0;
    memset(l->data, 0, cap * el_size);
    return l;
};

void ListDelete(list *l)
{
    free(l->data);
    free(l);
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
    elem[l->element_size - 1] = 0;
};

char *ListGetElement(list *l, int index)
{
    char *elem = l->data + (index * l->element_size);
    return elem;
};

list *GenerateFileList(buffer *buf)
{
    list *flist = ListCreate("Open:", 100, buf->panel.col_capacity - 5);
    
    tinydir_dir dir;
    tinydir_open(&dir, "e:/dev/ed");
    int count = 0;
    
    while (dir.has_next)
    {
        tinydir_file file;
        tinydir_readfile(&dir, &file);
        
        ListSetElement(flist, count, file.name);
        
        //printf("%s", file.name);
        //
        //if (file.is_dir)
        //{
        //printf("/");
        //}
        //printf("\n");
        
        tinydir_next(&dir);
        count++;
    }
    
    tinydir_close(&dir);
    
    return flist;
};

void PopulateFileList(list *l, char *path)
{
    tinydir_dir dir;
    tinydir_open(&dir, path);
    int count = 0;
    
    while (dir.has_next)
    {
        tinydir_file file;
        tinydir_readfile(&dir, &file);
        
        ListSetElement(l, count, file.name);
        
        tinydir_next(&dir);
        count++;
    }
    
    tinydir_close(&dir);
};