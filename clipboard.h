#pragma once
#include "globals.h"
#include "helpers.h"
#include "line.h"
#include "render.h"

void ClipBoardCopy(buffer *buf);
void ClipBoardCut(buffer *buf);
void ClipBoardPaste(buffer *buf);
void ClipBoardGetExternal();