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
        SDL_Color color = {0, 255, 0, 0};
    }cursor;
    
    struct _marker
    {
        int column = 0;
        int line = 0;
        SDL_Color color = {255, 0, 0, 0};
    }marker;
    
    struct _panel
    {
        panel_pos position;
        int x;
        int y;
        int w;
        int h;
        SDL_Color color = {100, 100, 100, 255};
    }panel;
};

struct font_data
{
    //FC_Font *name;
    TTF_Font *name;
    int width;
    int height;
};


font_data font;

node *headA = (node*)malloc(sizeof(node));
node *headB = (node*)malloc(sizeof(node));

buffer bufferA;
buffer bufferB;
