#pragma once
#include "globals.h"
#include "file.h"
#include "render.h"
#include "tinydir.h"
#include <string>

list *ListCreate(char title_text[], int cap, int el_size);
void ListDelete(list *l);
void ListClear(list *l);
void ListResize(list *l, int new_cap);
void ListSetElement(list *l, int index, char text[]);
char *ListGetElement(list *l, int index);
void ListSwitchToSelectedDirectory(list *l, char *sel_dir);
void ListLoadSelectedFile(list *l, char *sel_file);

void ListPopulate(list *l, char *path);
void ListApplyFilter(list *l, char *path);