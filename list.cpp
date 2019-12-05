#include "list.h"

list *CreateList(char title[], int elem_count, int max_elem_size)
{
    list *l = (list*)malloc(sizeof(list));
    
    memset(l->header, 0, 256);
    strcpy(l->header, title);
    
    l->item_count = elem_count;
    l->max_item_size = max_elem_size;
    l->data = (char*)malloc(elem_count * max_elem_size);
    memset(l->data, 0, elem_count * max_elem_size);
    return l;
};

void DeleteList(list *l)
{
    free(l->data);
    free(l);
};

void ListSetElement(list *l, int index, char text[])
{
    char *elem = l->data + (index * l->max_item_size);
    memset(elem, 0, l->max_item_size);
    memcpy(elem, text, l->max_item_size);
    elem[l->max_item_size - 1] = 0;
};

char *ListGetElement(list *l, int index)
{
    char *elem = l->data + (index * l->max_item_size);
    return elem;
};

//list *GenerateFileList(buffer *buf)
//{
//list *flist = CreateList("Open:", )
//};