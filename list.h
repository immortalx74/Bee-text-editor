#pragma once
#include "globals.h"
#include "tinydir.h"
#include <string>

list *ListCreate(char title_text[], int cap, int el_size);
void ListDelete(list *l);
void ListResize(list *l, int new_cap);
void ListSetElement(list *l, int index, char text[]);
char *ListGetElement(list *l, int index);

list *GenerateFileList(buffer *buf);