int margin = 4;

enum panel_pos
{
    LEFT,
    RIGHT
};

struct node
{
    char data[128] = {0};
    struct node *next;
    struct node *prev;
};


struct buffer
{
    node *head;
    int line_count;
    
    struct _cursor
    {
        int column = 0;
        int line = 0;
        SDL_Color color = {0, 255, 0, 255};
    }cursor;
    
    struct _marker
    {
        int column = 0;
        int line = 0;
        SDL_Color color = {255, 0, 0, 255};
    }marker;
    
    struct _panel
    {
        panel_pos position;
        int x;
        int y;
        int w;
        int h;
        SDL_Color color;
    }panel;
};

struct font_data
{
    FC_Font *name;
    int width;
    int height;
};

SDL_Color cur_col = {0, 255, 0, 255};
SDL_Color mar_col = {255, 0, 0, 255};
SDL_Color pan_col = {100, 100, 100, 255};

font_data font;

node *headA = (node*)malloc(sizeof(node));
node *headB = (node*)malloc(sizeof(node));

buffer bufferA;
buffer bufferB;




void CursorDraw(SDL_Renderer *renderer, buffer &buf)
{
    int xx = (buf.cursor.column * font.width) + margin + buf.panel.x;
    int yy = (buf.cursor.line * font.height) + margin;
    
    SDL_Rect box = {xx, yy, font.width, font.height};
    SDL_SetRenderDrawColor(renderer, buf.cursor.color.r, buf.cursor.color.g, buf.cursor.color.b, buf.cursor.color.a);
    SDL_RenderFillRect(renderer, &box);
};

void PanelDraw(SDL_Renderer *renderer, buffer &buf)
{
    SDL_Rect box = {buf.panel.x, buf.panel.y, buf.panel.w, buf.panel.h};
    SDL_SetRenderDrawColor(renderer, buf.panel.color.r, buf.panel.color.g, buf.panel.color.b, buf.panel.color.a);
    SDL_RenderDrawRect(renderer, &box);
}
