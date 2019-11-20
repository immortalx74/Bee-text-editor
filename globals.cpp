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
    TTF_Font *name;
    int width;
    int height;
};


font_data font;

node *headA = (node*)malloc(sizeof(node));
node *headB = (node*)malloc(sizeof(node));

buffer bufferA;
buffer bufferB;


struct app_info
{
    int ww;
    int wh;
    
    bool quit = false;
    SDL_Event e;
    char ascii_sequence[96];
    SDL_Window *window;
    SDL_Renderer *renderer;
    
    
    void Init()
    {
        SDL_Init(SDL_INIT_VIDEO);
        TTF_Init();
        
        window = SDL_CreateWindow("Ed", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,
                                  1024, 768, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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
        //bufferB.head = headB;
    };
};

app_info app;
SDL_Texture *characters_texture;
SDL_Texture *screen_texture;
SDL_Texture *im_texture;