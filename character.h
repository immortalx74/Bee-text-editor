#pragma once
#include "globals.h"
#include "line.h"
#include "draw.h"
#include "SDL_FontCache.h"
#include <string>

void InsertCharacterAt(buffer *buf, node *row, int col);
node *DeleteCharacterAt(buffer *buf, node *row, int col);