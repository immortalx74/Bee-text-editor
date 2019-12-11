#pragma once
#include "SDL.h"
#include "SDL_ttf.h"

extern int margin;

struct node
{
    char data[256] = {0};
    struct node *next;
    struct node *prev;
};

struct list
{
    char title[256] = {0};
    int selected = 0;
    int capacity;
    int element_size = 0;
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

struct font_data
{
    TTF_Font *name;
    int width;
    int height;
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
    SDL_Event e;
    char ascii_sequence[96];
    SDL_Window *window;
    SDL_Renderer *renderer;
    buffer *active_buffer;
    _mode mode = TEXT_EDIT;
    
    void Init()
    {
        SDL_Init(SDL_INIT_VIDEO);
        TTF_Init();
        
        font.name = TTF_OpenFont("liberation-mono.ttf", 13);
        font.height = TTF_FontHeight(font.name);
        TTF_SizeText(font.name, "A", &font.width, 0);
        
        window = SDL_CreateWindow("Ed", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,
                                  1024, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        
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

extern app_info app;
extern SDL_Texture *characters_texture;
extern SDL_Texture *bar_characters_texture;
extern SDL_Texture *screen_texture;
