#pragma once
#include "SDL.h"
#include "SDL_ttf.h"
#include <string.h>

#define LINE_MEM_CHUNK 10
#define MARGIN 4
#define UNDO_STEPS 50

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

struct node
{
    char *data;
    int num_chunks;
    struct node *next;
    struct node *prev;
};

struct xstring
{
    char *data;
    int length;
};

struct _clipboard
{
    xstring *text;
    bool has_content;
};

struct list
{
    int selected = 0;
    int capacity;
    int element_count = 0;
    int element_size = 0;
    int scroll_offset = 0;
    int row = 0;
    xstring *title;
    xstring *current_path;
    xstring *filter;
    char *data;
};

struct buffer
{
    node *head;
    node *line_node;
    int line_count;
    int column;
    int line;
    char filename[128] = "*no file*";
    list *lst = NULL;
    
    struct _cursor
    {
        int col;
        int row;
        int last_hor_pos = 0;
        SDL_Color color = {0, 255, 0, 0};
        SDL_Color line_highlight = {40, 0, 180, 255};
    }cursor;
    
    struct _marker
    {
        int col = 0;
        int row = 0;
        int bufcol = 0;
        int bufrow = 0;
        SDL_Color color = {255, 0, 0, 0};
    }marker;
    
    struct _panel
    {
        int x;
        int y;
        int w;
        int h;
        int row_capacity;
        int col_capacity;
        int page = 0;
        int scroll_offset_hor;
        int scroll_offset_ver;
        SDL_Texture *texture;
        SDL_Color color = {100, 100, 100, 255};
    }panel;
    
    struct _status_bar
    {
        int x;
        int y;
        int w;
        int h;
        char info[128] = {0};
        SDL_Texture *texture;
        SDL_Color color = {140, 140, 140, 255};
        SDL_Color text_color = {10, 10, 10, 255};
    }status_bar;
    
};

enum op_type
{
    OP_INSERT,
    OP_DELETE,
    OP_LINE_MERGE,
    OP_LINE_SPLIT
};

struct undoredo_op
{
    buffer *buf;
    op_type type;
    int row;
    int col;
    bool locked;
    xstring *text;
};

extern undoredo_op *undo_stack;
extern int undoredo_index;

#define UNDOREDO_IDX undoredo_index = undoredo_index % UNDO_STEPS
#define UNDOREDO_INC undoredo_index++
#define UNDOREDO_DEC undoredo_index--

inline int UNDOREDO_MINUS()
{
    if(undoredo_index <= 0)
    {
        return undoredo_index + UNDO_STEPS;
    }
    
    return undoredo_index - 1;
};

struct font_data
{
    TTF_Font *name;
    int width;
    int height;
    int size = 13;
    SDL_Color text_color = {143, 175, 127, 255};
    SDL_Color text_color_inv = {5, 5, 5, 255};
};

extern font_data font;

extern node *headA;
extern node *headB;

extern buffer bufferA;
extern buffer bufferB;


enum _mode
{
    TEXT_EDIT,
    LIST_NAV
};

struct app_info
{
    int ww;
    int wh;
    
    bool quit = false;
    bool idle = false;
    SDL_Event e;
    char ascii_sequence[96];
    SDL_Window *window;
    SDL_Renderer *renderer;
    buffer *active_buffer;
    _mode mode = TEXT_EDIT;
    xstring *last_path;
    SDL_BlendMode custom_blendmode;
    
    void Init()
    {
        SDL_Init(SDL_INIT_VIDEO);
        TTF_Init();
        
        font.name = TTF_OpenFont("liberation-mono.ttf", font.size);
        font.height = TTF_FontHeight(font.name);
        TTF_SizeText(font.name, "A", &font.width, 0);
        
        window = SDL_CreateWindow("Ed", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,
                                  1024, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE/* | SDL_WINDOW_MAXIMIZED*/);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        
        custom_blendmode = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ONE_MINUS_DST_COLOR,
                                                      SDL_BLENDFACTOR_ZERO,
                                                      SDL_BLENDOPERATION_ADD,
                                                      SDL_BLENDFACTOR_ONE,
                                                      SDL_BLENDFACTOR_ONE,
                                                      SDL_BLENDOPERATION_ADD);
        
        // This blendmode results in black cursor with green text
        //app.custom_blendmode = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_DST_COLOR,
        //SDL_BLENDFACTOR_SRC_ALPHA,
        //SDL_BLENDOPERATION_ADD,
        //SDL_BLENDFACTOR_ZERO,
        //SDL_BLENDFACTOR_ONE,
        //SDL_BLENDOPERATION_ADD);
        
        SDL_GetWindowSize(window, &ww, &wh);
        
        for (int i = 0; i <= 95; ++i)
        {
            ascii_sequence[i] = 32 + i;
        }
        ascii_sequence[95] = '\0';
        
        // Init dummy heads
        headA->prev = NULL;
        headA->next = NULL;
        headB->prev = NULL;
        headB->next = NULL;
        
        bufferA.head = headA;
        bufferB.head = headB;
    };
};

extern _clipboard clipboard;
extern app_info app;
extern SDL_Texture *characters_texture;
extern SDL_Texture *bar_characters_texture;
extern SDL_Texture *screen_texture;