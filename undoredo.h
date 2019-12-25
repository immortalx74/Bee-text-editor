#pragma once
#include "globals.h"
#include "helpers.h"

void UndoStackStoreOp(buffer *buf, op_type t, int row, int col, char *text);