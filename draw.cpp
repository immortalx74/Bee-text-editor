#include "draw.h"

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
    SDL_GetWindowSize(win, &application->ww, &application->wh);
    
    bufferA.panel.x = 1;
    bufferA.panel.y = 1;
    bufferA.panel.w = (application->ww / 2) - 1;
    bufferA.panel.h = application->wh - 1;
    
    bufferB.panel.x = (application->ww / 2) + 1;
    bufferB.panel.y = 1;
    bufferB.panel.w = (application->ww / 2) - 1;
    bufferB.panel.h = application->wh - 1;
};

void RenderCharacterAt(node *line_node, int row, int col, int row_length, SDL_Texture *ch, SDL_Texture *im)
{
    SDL_SetRenderTarget(app.renderer, im);
    int cur_char = (int)app.e.text.text[0];
    SDL_Rect glyph_rect = {(cur_char - 32) * font.width, 0, font.width, font.height};
    
    if(col == row_length - 1) // draw last character in line
    {
        SDL_Rect pos = {margin + (col * font.width), margin + (row * font.height), font.width, font.height};
        SDL_RenderCopy(app.renderer, ch, &glyph_rect, &pos);
    }
    else //draw current character & trailing characters
    {
        int chars_to_clear = row_length - col;
        SDL_Rect clear_rect = {margin + (col * font.width), margin + (row * font.height), chars_to_clear * font.width, font.height};
        SDL_RenderFillRect(app.renderer, &clear_rect);
        
        for (int i = col; i < row_length; ++i)
        {
            int cur_char = (int)line_node->data[i];
            SDL_Rect glyph_rect = {(cur_char - 32) * font.width, 0, font.width, font.height};
            SDL_Rect pos = {margin + (i * font.width), margin + (bufferA.cursor.line * font.height), font.width, font.height};
            
            SDL_RenderCopy(app.renderer, ch, &glyph_rect, &pos);
        }
    }
    
    SDL_SetRenderTarget(app.renderer, NULL);
};

void RenderClearCharacterAt(node *line_node, int row, int col, int row_length, SDL_Texture *ch, SDL_Texture *im)
{
    SDL_SetRenderTarget(app.renderer, im);
    SDL_SetRenderDrawColor(app.renderer, 21, 12, 42, 0);// background
    int cur_char = (int)line_node->data[col];
    int chars_to_clear = row_length + 1 - col;
    SDL_Rect glyph_rect = {(cur_char - 32) * font.width, 0, font.width, font.height};
    
    SDL_Rect clear_rect = {margin + (col * font.width), margin + (row * font.height), chars_to_clear * font.width, font.height};
    SDL_RenderFillRect(app.renderer, &clear_rect);
    
    
    if(col < row_length)
    {
        for (int i = col; i < row_length; ++i)
        {
            int cur_char = (int)line_node->data[i];
            glyph_rect = {(cur_char - 32) * font.width, 0, font.width, font.height};
            SDL_Rect pos = {margin + (i * font.width), margin + (bufferA.cursor.line * font.height), font.width, font.height};
            
            SDL_RenderCopy(app.renderer, ch, &glyph_rect, &pos);
        }
    }
    SDL_SetRenderTarget(app.renderer, NULL);
};

void RenderClearLine(buffer *buf, node *line_node, int row, SDL_Texture *ch, SDL_Texture *im)
{
    SDL_SetRenderTarget(app.renderer, im);
    SDL_SetRenderDrawColor(app.renderer, 21, 12, 42, 0);// background
    
    int cur_char = (int)line_node->data[0];
    
    SDL_Rect glyph_rect = {(cur_char - 32) * font.width, 0, font.width, font.height};
    
    SDL_Rect clear_rect = {margin , margin + ((row) * font.height), buf->panel.w, (buf->line_count + 1) * font.height};
    SDL_RenderFillRect(app.renderer, &clear_rect);
    
    for (int i = row; i < buf->line_count; ++i)
    {
        for (int j = 0; j < strlen(line_node->data); ++j)
        {
            cur_char = (int)line_node->data[j];
            SDL_Rect glyph_rect = {(cur_char - 32) * font.width, 0, font.width, font.height};
            SDL_Rect pos = {margin + (j * font.width), margin + (i * font.height), font.width, font.height};
            SDL_RenderCopy(app.renderer, ch, &glyph_rect, &pos);
        }
        
        if(line_node->next != NULL)
        {
            line_node = line_node->next;
        }
        
    }
    
    SDL_SetRenderTarget(app.renderer, NULL);
};