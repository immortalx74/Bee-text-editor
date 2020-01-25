#pragma once
#include "SDL.h"
#include "SDL_ttf.h"
#include <string.h>
#include <iostream>
#include "resource.h"


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

struct key_binding
{
    SDL_Keycode key;
    int mod1;
    int mod2;
};

enum sets_enum
{
    font_name,
    start_path,
    font_size,
    tab_size,
    line_mem_chunk,
    margin,
    undo_steps,
    cursor_blink,
    cursor_blink_rate,
    draw_line_highlight,
    color_background,
    color_panel_outline,
    color_text,
    color_line_highlight,
    color_cursor,
    color_marker,
    color_bar_background,
    color_bar_text,
    
    command_file_open,
    command_file_save,
    command_marker_set,
    command_buffer_kill,
    command_buffer_toggle_active
};

struct _settings
{
    xstring *font_name;
    xstring *start_path;
    int font_size;
    int tab_size;
    int line_mem_chunk;
    int margin;
    int undo_steps;
    bool cursor_blink;
    int cursor_blink_rate;
    bool draw_line_highlight;
    SDL_Color color_background;
    SDL_Color color_panel_outline;
    SDL_Color color_text;
    SDL_Color color_line_highlight;
    SDL_Color color_cursor;
    SDL_Color color_marker;
    SDL_Color color_bar_background;
    SDL_Color color_bar_text;
    
    key_binding kb_command_file_open;
    key_binding kb_command_file_save;
    key_binding kb_command_marker_set;
    key_binding kb_command_buffer_kill;
    key_binding kb_command_buffer_toggle_active;
};

extern _settings settings;

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
    xstring *filename;
    list *lst = NULL;
    
    struct _cursor
    {
        int col;
        int row;
        int last_hor_pos = 0;
        SDL_Color color = {0, 255, 0, 255};
        SDL_Color line_highlight = {40, 0, 180, 255};
        bool visible = true;
        bool blink_state_on = true;
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
extern int undoredo_counter;

#define UNDOREDO_IDX (undoredo_index = (undoredo_index % settings.undo_steps))
#define UNDOREDO_INC undoredo_index++
#define UNDOREDO_DEC undoredo_index--

inline int UNDOREDOISNEGATIVE()
{
    if(undoredo_index < 0)
    {
        return undoredo_index + settings.undo_steps;
    }
    return undoredo_index;
};

struct font_data
{
    TTF_Font *handle;
    xstring *name;
    int width;
    int height;
    int size;
    SDL_Color text_color = {143, 175, 127, 255};
};

extern font_data font;

extern node *headA;
extern node *headB;

extern buffer bufferA;
extern buffer bufferB;

enum direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

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
};

struct _timer
{
    int interval = 1000;
    unsigned int start_time = 0;
    unsigned int now = 0;
    bool running = false;
    
    void reset()
    {
        running = true;
        start_time = SDL_GetTicks();
    };
};

extern _timer timer;
extern _clipboard clipboard;
extern app_info app;
extern SDL_Texture *characters_texture;
extern SDL_Texture *bar_characters_texture;
extern SDL_Texture *screen_texture;


void Init();