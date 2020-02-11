#pragma once
#include "globals.h"
#include "helpers.h"
#include "render.h"

void UndoRedo_AllocateBatch(buffer *buf);
void UndoRedo_StoreOp(buffer *buf, undoredo_op_type t, int row, int col, char *text);




//undoredo_op *UndoRedo_CreateOp(void);
//undoredo_op *UndoRedo_GetNextSlot(void);
//void UndoRedo_StoreOp(buffer *buf, undoredo_op_type t, int row, int col, char *text);