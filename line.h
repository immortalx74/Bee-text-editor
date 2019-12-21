#pragma once
#include "globals.h"
#include <string>
#include "character.h"

void LineRequestMemChunks(node *ln, int num);
void LineShrinkMemChunks(node *ln);
void LineEnsureSufficientCapacity(node *ln);
node *CreateLine(void);
void InsertLineAt(buffer *buf, int pos);
void DeleteLineAt(buffer *buf, int pos);
void AttemptSetToLastColumn(buffer *buf);
node *GetLineNode(buffer *buf, int pos);
node *KillBuffer(buffer *buf);
void ClipBoardCopy();
void ClipBoardPaste();
void ClipBoardGetExternal();