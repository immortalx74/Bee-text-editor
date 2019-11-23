#pragma once
#include "SDL.h"
#include "globals.h"
#include <string>

void CursorDraw(SDL_Renderer *renderer, buffer *buf);
void PanelDraw(SDL_Renderer *renderer, buffer *buf);
void WindowResize(app_info *application, SDL_Window *win);
void RenderCharacterAt(buffer *buf, node *line_node, int row, int col, int row_length, SDL_Texture *ch, SDL_Texture *im);
void RenderClearCharacterAt(buffer *buf, node *line_node, int row, int col, int row_length, SDL_Texture *ch, SDL_Texture *im);
void RenderClearLine(buffer *buf, node *line_node, int row, SDL_Texture *ch, SDL_Texture *im);