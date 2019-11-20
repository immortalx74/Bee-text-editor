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
};

void WindowResize(app_info *application, SDL_Window *win)
{
    int ww = application->ww;
    int wh = application->wh;
    
    SDL_GetWindowSize(win, &ww, &wh);
    
    bufferA.panel.x = 1;
    bufferA.panel.y = 1;
    bufferA.panel.w = (ww / 2) - 1;
    bufferA.panel.h = wh - 1;
    
    bufferB.panel.x = (ww / 2) + 1;
    bufferB.panel.y = 1;
    bufferB.panel.w = (ww / 2) - 1;
    bufferB.panel.h = wh - 1;
};

void RenderCharacterAt(int row, int col, int row_length, SDL_Texture *ch, SDL_Texture *im)
{
    SDL_SetRenderTarget(app.renderer, im);
    int cur_char_code = (int)app.e.text.text[0];
    SDL_Rect glyph_rect = {(cur_char_code - 32)*font.width,0,font.width,font.height};
    
    if(col == row_length - 1) // append
    {
        SDL_Rect pos = {margin+(col*font.width),margin+(row*font.height),font.width,font.height};
        SDL_RenderCopy(app.renderer, ch, &glyph_rect, &pos);
    }
    else
    {
        int chars_to_clear = row_length - col;
        SDL_Rect clear = {margin+(col*font.width),margin+(row*font.height),chars_to_clear * font.width,font.height};
        SDL_RenderFillRect(app.renderer, &clear);
    }
    
    SDL_SetRenderTarget(app.renderer, NULL);
};