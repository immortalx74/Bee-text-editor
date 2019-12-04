#pragma once
#include "globals.h"
#include <string>

node *CreateLine(void);
node *InsertLineAt(buffer *buf, int pos);
void DeleteLineAt(buffer *buf, int pos);
void AttemptSetToLastColumn(buffer *buf, node *cur_node);
node *GetLineNode(buffer *buf, int pos);
node *KillBuffer(buffer *buf);