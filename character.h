#pragma once
#include "globals.h"
#include "line.h"
#include "draw.h"
#include "undoredo.h"
#include <string>

void InsertCharacterAt(buffer *buf, int col);
void DeleteCharacterAt(buffer *buf, int col);