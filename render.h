#pragma once
#include "SDL.h"
#include "globals.h"
#include "helpers.h"
#include "list.h"
#include "character.h"
#include <string>

void RenderCursor(buffer *buf);
void RenderMarker(buffer *buf);
void RenderLineHighlight(buffer *buf);
void RenderPanel(buffer *buf);
void RenderStatusBar(buffer *buf);
void WindowResize(app_info *application, SDL_Window *win);
void RenderCharacter(buffer *buf, int row, int col, int row_length, SDL_Texture *ch, SDL_Texture *pt);
void RenderClearCharacter(buffer *buf, int row, int col, int row_length, SDL_Texture *ch, SDL_Texture *pt);
void RenderClearLine(buffer *buf, int row, SDL_Texture *ch, SDL_Texture *pt);
void RenderLineRange(buffer *buf, int start, int end, SDL_Texture *ch, SDL_Texture *pt);
void SyncCursorWithBuffer(buffer *buf);
void SwitchHorizontalPage(buffer *buf);
bool MarkerIsWithinDrawingBounds(buffer *buf);

void RenderList(buffer *buf, list *l, SDL_Texture *ch, SDL_Texture *pt);
void RenderListRange(buffer *buf, int start, int count, SDL_Texture *ch, SDL_Texture *pt);

void RenderListElementHighlight(buffer *buf, list *l);
void PanelsResize(int mousex, SDL_Cursor *c);
void CursorSetToMouse(buffer *buf, int mousex, int mousey);