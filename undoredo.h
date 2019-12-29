#pragma once
#include "globals.h"
#include "helpers.h"
#include "draw.h"

void UndoStackStoreOp(buffer *buf, op_type t, int row, int col, char *text);
void UndoStackCommitUndo(buffer *buf);
void UndoStackCommitRedo(buffer *buf);

void UndoOpInsert(buffer *buf, undoredo_op op);
void UndoOpDelete(buffer *buf, undoredo_op op);
void RedoOpInsert(buffer *buf, undoredo_op op);
void RedoOpDelete(buffer *buf, undoredo_op op);
