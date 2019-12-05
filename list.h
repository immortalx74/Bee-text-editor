#pragma once
#include "globals.h"
#include <string>

list *CreateList(char title[], int elem_count, int max_elem_size);
void DeleteList(list *l);
void ListSetElement(list *l, int index, char text[]);
char *ListGetElement(list *l, int index);