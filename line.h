#pragma once
#include "globals.h"
#include <string>

void LineRequestMemChunks(node *ln, int num);
void LineShrinkMemChunks(node *ln);
void LineExpandMemChunks(node *ln, int new_len);
void LineEnsureSufficientCapacity(node *ln);
node *CreateLine(void);
void InsertLineAt(buffer *buf, int pos);
void DeleteLineAt(buffer *buf, int pos);
void AttemptSetToLastColumn(buffer *buf);
node *GetLineNode(buffer *buf, int pos);
node *KillBuffer(buffer *buf);