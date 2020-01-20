#include "globals.h"

undoredo_op *undo_stack = (undoredo_op*)malloc(settings.undo_steps * sizeof(undoredo_op));
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

void Init()
{
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    font.handle = TTF_OpenFont(font.name->data, font.size);
    font.height = TTF_FontHeight(font.handle);
    TTF_SizeText(font.handle, "0", &font.width, &font.height);
    
    app.window = SDL_CreateWindow("ed", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,
                                  1024, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE /*| SDL_WINDOW_MAXIMIZED*/);
    app.renderer = SDL_CreateRenderer(app.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    // original. works somewhat OK
    //app.custom_blendmode = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ONE_MINUS_DST_COLOR,
    //SDL_BLENDFACTOR_ZERO,
    //SDL_BLENDOPERATION_ADD,
    //SDL_BLENDFACTOR_ONE,
    //SDL_BLENDFACTOR_ONE,
    //SDL_BLENDOPERATION_ADD);
    
    // This blendmode results in black cursor with green text
    //app.custom_blendmode = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_DST_COLOR,
    //SDL_BLENDFACTOR_SRC_ALPHA,
    //SDL_BLENDOPERATION_ADD,
    //SDL_BLENDFACTOR_ZERO,
    //SDL_BLENDFACTOR_ONE,
    //SDL_BLENDOPERATION_ADD);
    
    //That one seems the best
    //app.custom_blendmode = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ONE_MINUS_DST_COLOR,
    //SDL_BLENDFACTOR_ONE_MINUS_SRC_COLOR,
    //SDL_BLENDOPERATION_ADD,
    //SDL_BLENDFACTOR_ONE,
    //SDL_BLENDFACTOR_ONE,
    //SDL_BLENDOPERATION_ADD);
    
    //Even better?
    app.custom_blendmode = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ONE_MINUS_DST_COLOR,
                                                      SDL_BLENDFACTOR_ZERO,
                                                      SDL_BLENDOPERATION_ADD,
                                                      SDL_BLENDFACTOR_ONE,
                                                      SDL_BLENDFACTOR_ZERO,
                                                      SDL_BLENDOPERATION_ADD);
    
    SDL_GetWindowSize(app.window, &app.ww, &app.wh);
    
    for (int i = 0; i <= 95; ++i)
    {
        app.ascii_sequence[i] = 32 + i;
    }
    app.ascii_sequence[95] = '\0';
    
    // Init dummy heads
    headA->prev = NULL;
    headA->next = NULL;
    headB->prev = NULL;
    headB->next = NULL;
    
    bufferA.head = headA;
    bufferB.head = headB;
};