#pragma once
#include "SDL.h"
#include "globals.h"
#include "helpers.h"
#include "list.h"
#include <string>

void CursorDraw(buffer *buf);
void MarkerDraw(buffer *buf);
void HighlightLineDraw(buffer *buf);
void PanelDraw(buffer *buf);
void BarDraw(buffer *buf);
void WindowResize(app_info *application, SDL_Window *win);
void RenderCharacterAt(buffer *buf, int row, int col, int row_length, SDL_Texture *ch, SDL_Texture *pt);
void RenderClearCharacterAt(buffer *buf, int row, int col, int row_length, SDL_Texture *ch, SDL_Texture *pt);
void RenderClearLine(buffer *buf, int row, SDL_Texture *ch, SDL_Texture *pt);
void RenderLineRange(buffer *buf, int start, int end, SDL_Texture *ch, SDL_Texture *pt);
void SyncCursorWithBuffer(buffer *buf);
void SwitchHorizontalPage(buffer *buf);

void ListDraw(buffer *buf, list *l, SDL_Texture *ch, SDL_Texture *pt);
void RenderListRange(buffer *buf, int start, int count, SDL_Texture *ch, SDL_Texture *pt);

void HighlightListSelectionDraw(buffer *buf, list *l);