#pragma once
#include "globals.h"
#include "file.h"

void InputText(buffer *buf);
void InputReturn(buffer *buf);
void InputBackspace(buffer *buf);
void InputDelete(buffer *buf);
void InputLeft(buffer *buf);
void InputRight(buffer *buf);
void InputUp(buffer *buf);
void InputDown(buffer *buf);
void InputPageUp(buffer *buf);
void InputPageDown(buffer *buf);
void InputHome(buffer *buf);
void InputEnd(buffer *buf);
void InputTab(buffer *buf);

void ListNav_Down(list *l);

void GetTextEditingInput();
void GetBindedCommandsInput();
void GetListNavigationInput();
void GetGlobalInput();