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