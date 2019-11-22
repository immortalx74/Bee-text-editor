#pragma once
#include "globals.h"
#include <string>
//#include "print.h"

node *CreateLine(void);
node *InsertLineAt(buffer *buf, int pos);
void DeleteLineAt(buffer *buf, int pos);
//void PrintData(node *head_node);