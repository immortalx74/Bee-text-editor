#pragma once
#include "globals.h"
#include "file.h"
#include "draw.h"
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

void PopulateFileList(list *l, char *path);
void FilterFileList(list *l, char *path);