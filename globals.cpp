#include "globals.h"

int margin = 4;

font_data font;
node *headA = (node*)malloc(sizeof(node));
node *headB = (node*)malloc(sizeof(node));

buffer bufferA;
buffer bufferB;

app_info app;
SDL_Texture *characters_texture;
SDL_Texture *screen_texture;
SDL_Texture *im_texture;