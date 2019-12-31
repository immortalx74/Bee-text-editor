#include "globals.h"

undoredo_op *undo_stack = (undoredo_op*)malloc(UNDO_STEPS * sizeof(undoredo_op));
int undoredo_index = -1;
int undoredo_counter = 0;

font_data font;
node *headA = (node*)malloc(sizeof(node));
node *headB = (node*)malloc(sizeof(node));

buffer bufferA;
buffer bufferB;

_clipboard clipboard;
app_info app;
SDL_Texture *characters_texture;
SDL_Texture *bar_characters_texture;
SDL_Texture *screen_texture;

_settings settings;