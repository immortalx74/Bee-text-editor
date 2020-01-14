#include "list.h"

list *ListCreate(char title_text[], int cap, int el_size)
{
    list *l = (list*)malloc(sizeof(list));
    
    l->title = XstringCreate(title_text);
    l->current_path = XstringCreate(XstringGet(app.last_path));
    l->filter = XstringCreate("");
    
    l->selected = 0;
    l->capacity = cap;
    l->element_count = 0;
    l->element_size = el_size;
    l->scroll_offset = 0;
    l->row = 0;
    
    l->data = (char*)malloc(cap * el_size);
    memset(l->data, 0, cap * el_size);
    
    return l;
};

void ListDelete(list *l)
{
    XstringDestroy(l->title);
    XstringDestroy(l->current_path);
    XstringDestroy(l->filter);
    free(l->data);
    free(l);
};

void ListClear(list *l)
{
    free(l->data);
    
    l->data = (char*)malloc(l->capacity * l->element_size);
    memset(l->data, 0, l->capacity * l->element_size);
    
    l->selected = 0;
    l->row = 0;
    l->scroll_offset = 0;
};

void ListResize(list *l, int new_cap)
{
    l->data = (char*)realloc(l->data, new_cap * l->element_size);
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

void ListPopulate(list *l, char *path)
{
    tinydir_dir dir;
    tinydir_open(&dir, path);
    int count = 0;
    l->element_count= 0;
    XstringSet(l->current_path, path);
    char *current;
    
    while (dir.has_next)
    {
        tinydir_file file;
        tinydir_readfile(&dir, &file);
        
        current = file.name;
        
        if(!IsSymLink(current))
        {
            if (file.is_dir)
            {
                int len = strlen(current);
#ifdef _WIN32
                current[len] = '\\';
#endif
#ifdef __linux__
                current[len] = '/';
#endif
                current[len + 1] = '\0';
            }
            
            if(l->element_count + 1 >= l->capacity)
            {
                ListResize(l, l->capacity * 2);
            }
            
            ListSetElement(l, count, current);
            l->element_count++;
            
            count++;
        }
        
        tinydir_next(&dir);
    }
    
    tinydir_close(&dir);
};

void ListApplyFilter(list *l, char *path)
{
    tinydir_dir dir;
    tinydir_open(&dir, path);
    int count = 0;
    l->element_count = 0;
    XstringSet(l->current_path, path);
    char *current;
    
    while (dir.has_next)
    {
        tinydir_file file;
        tinydir_readfile(&dir, &file);
        
        current = file.name;
        
        if(strstr(current, XstringGet(l->filter)) != NULL)
        {
            if(!IsSymLink(current))
            {
                if (file.is_dir)
                {
                    int len = strlen(current);
#ifdef _WIN32
                    current[len] = '\\';
#endif
#ifdef __linux__
                    current[len] = '/';
#endif
                    current[len + 1] = '\0';
                }
                
                ListSetElement(l, count, current);
                l->element_count++;
                count++;
            }
        }
        
        tinydir_next(&dir);
    }
    
    tinydir_close(&dir);
};

void ListSwitchToSelectedDirectory(list *l, char *sel_dir)
{
    XstringConcat(l->current_path, 1, sel_dir);
    ListClear(l);
    XstringSet(l->filter, "");
    ListPopulate(l, XstringGet(l->current_path));
};

void ListLoadSelectedFile(list *l, char *sel_file)
{
    XstringSet(app.last_path, XstringGet(l->current_path));
    
    XstringConcat(l->current_path, 1, sel_file);
    
    FileReadToBuffer(app.active_buffer, XstringGet(l->current_path));
    
    if(app.active_buffer->lst != NULL)
    {
        ListDelete(app.active_buffer->lst);
        app.active_buffer->lst = NULL;
        app.mode = TEXT_EDIT;
        RenderLineRange(app.active_buffer, app.active_buffer->panel.scroll_offset_ver, app.active_buffer->panel.row_capacity, characters_texture, app.active_buffer->panel.texture);
    }
};
