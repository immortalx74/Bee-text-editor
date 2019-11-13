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
    int margin = 4;
};

struct buffer
{
    int line_count;
    int current_panel;
};

struct cursor
{
    int col = 0;
    int line = 0;
};

struct font_data
{
    FC_Font *name;
    int width;
    int height;
};

font_data font;

node *headL = (node*)malloc(sizeof(node));
node *headR = (node*)malloc(sizeof(node));

panel panelL;
panel panelR;

buffer bufferL = {0, LEFT};
buffer bufferR = {0, RIGHT};

cursor cursorL;
cursor cursorR;
cursor markerL;
cursor markerR;