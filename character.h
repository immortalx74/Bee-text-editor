#pragma once
#include "globals.h"
#include "line.h"
#include "render.h"
#include "undoredo.h"
#include <string>

void CharacterInsert(buffer *buf, int col);
void CharacterDelete(buffer *buf, int col);