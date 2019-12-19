#pragma once
#include "globals.h"
#include <string>

void LineRequestMemChunk(node *ln, int num);
node *CreateLine(void);
void InsertLineAt(buffer *buf, int pos);
void DeleteLineAt(buffer *buf, int pos);
void AttemptSetToLastColumn(buffer *buf);
node *GetLineNode(buffer *buf, int pos);
node *KillBuffer(buffer *buf);