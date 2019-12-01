#pragma once
#include "SDL.h"
#include "globals.h"
#include "helpers.h"
#include <string>

void CursorDraw(SDL_Renderer *renderer, buffer *buf);
void HighlightLineDraw(SDL_Renderer *renderer, buffer *buf);
void PanelDraw(SDL_Renderer *renderer, buffer *buf);
void BarDraw(SDL_Renderer *renderer, buffer *buf);
void WindowResize(app_info *application, SDL_Window *win);
void RenderCharacterAt(buffer *buf, node *line_node, int row, int col, int row_length, SDL_Texture *ch, SDL_Texture *pt);
void RenderClearCharacterAt(buffer *buf, node *line_node, int row, int col, int row_length, SDL_Texture *ch, SDL_Texture *pt);
void RenderClearLine(buffer *buf, node *line_node, int row, SDL_Texture *ch, SDL_Texture *pt);
void RenderLineRange(buffer *buf, int start, int end, SDL_Texture *ch, SDL_Texture *pt);
void SyncCursorWithBuffer(buffer *buf);
void SwitchHorizontalPage(buffer *buf);