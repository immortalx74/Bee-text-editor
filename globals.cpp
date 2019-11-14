int margin = 4;

enum
{
    LEFT,
    RIGHT,
    UP,
    DOWN
};

struct node
{
    char data[128] = {0};
    struct node *next;
    struct node *prev;
};

struct panel
{
    int x;
    int y;
    int w;
    int h;
    int colR;
    int colG;
    int colB;
    int colA;
};

struct buffer
{
    int line_count;
    int current_panel;
};

struct font_data
{
    FC_Font *name;
    int width;
    int height;
};

struct cursor
{
    int col = 0;
    int line = 0;
    int colR;
    int colG;
    int colB;
    int colA;
};

font_data font;

panel panelL = {0, 0, 0, 0, 100, 100, 100, 255};
panel panelR = {0, 0, 0, 0, 100, 100, 100, 255};

node *headL = (node*)malloc(sizeof(node));
node *headR = (node*)malloc(sizeof(node));

buffer bufferL = {0, LEFT};
buffer bufferR = {0, RIGHT};

cursor cursorL = {0, 0, 0, 255, 0, 255};
cursor cursorR = {0, 0, 0, 255, 0, 255};
cursor markerL = {0, 0, 255, 0, 0, 255};
cursor markerR = {0, 0, 255, 0, 0, 255};


void CursorDraw(SDL_Renderer *renderer, cursor *cur, panel *pan)
{
    int xx = (cur->col * font.width) + margin + pan->x;
    int yy = (cur->line * font.height) + margin;
    
    SDL_Rect box = {xx, yy, font.width, font.height};
    SDL_SetRenderDrawColor(renderer, cur->colR, cur->colG, cur->colB, cur->colA);
    
    if(cur == &cursorL || cur == &cursorR)
    {
        SDL_RenderFillRect(renderer, &box);
    }
    else
    {
        SDL_RenderDrawRect(renderer, &box);
    }
};

void PanelDraw(SDL_Renderer *renderer, panel *pan)
{
    SDL_Rect box = {pan->x, pan->y, pan->w, pan->h};
    SDL_SetRenderDrawColor(renderer, pan->colR, pan->colG, pan->colB, pan->colA);
    SDL_RenderDrawRect(renderer, &box);
}
