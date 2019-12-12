#pragma once
#include "globals.h"
#include "file.h"

void Input_TextEd_Text(buffer *buf);
void Input_TextEd_Return(buffer *buf);
void Input_TextEd_Backspace(buffer *buf);
void Input_TextEd_Delete(buffer *buf);
void Input_TextEd_Left(buffer *buf);
void Input_TextEd_Right(buffer *buf);
void Input_TextEd_Up(buffer *buf);
void Input_TextEd_Down(buffer *buf);
void Input_TextEd_PageUp(buffer *buf);
void Input_TextEd_PageDown(buffer *buf);
void Input_TextEd_Home(buffer *buf);
void Input_TextEd_End(buffer *buf);
void Input_TextEd_Tab(buffer *buf);

void Input_ListNav_Down(list *l);
void Input_ListNav_Up(list *l);
void Input_ListNav_Select(list *l);
void Input_ListNav_ParentDirectory(list *l);
void Input_ListNav_Abort(list *l);

void GetTextEditingInput();
void GetBindedCommandsInput();
void GetListNavigationInput();
void GetGlobalInput();