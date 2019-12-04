#include "draw.h"
#include "character.h"
#include <iostream>

void CursorDraw(SDL_Renderer *renderer, buffer *buf)
{
    // NOTE xx now takes page into account
    int xx = ((buf->cursor.col - buf->panel.page * buf->panel.col_capacity) * font.width) + margin + buf->panel.x;
    int yy = (buf->cursor.row * font.height) + margin;
    
    SDL_Rect box = {xx, yy, font.width, font.height};
    SDL_SetRenderDrawColor(renderer, buf->cursor.color.r, buf->cursor.color.g, buf->cursor.color.b, buf->cursor.color.a);
    SDL_RenderFillRect(renderer, &box);
};

void PanelDraw(SDL_Renderer *renderer, buffer *buf)
{
    SDL_Rect box = {buf->panel.x, buf->panel.y, buf->panel.w, buf->panel.h};
    SDL_SetRenderDrawColor(renderer, buf->panel.color.r, buf->panel.color.g, buf->panel.color.b, buf->panel.color.a);
    SDL_RenderDrawRect(renderer, &box);
};

void BarDraw(SDL_Renderer *renderer, buffer *buf)
{
    SDL_Rect box = {buf->status_bar.x, buf->status_bar.y, buf->status_bar.w, buf->status_bar.h};
    SDL_SetRenderDrawColor(renderer, buf->panel.color.r, buf->panel.color.g, buf->panel.color.b, buf->panel.color.a);
    SDL_RenderDrawRect(renderer, &box);
    
    //draw info
    SDL_SetRenderTarget(app.renderer, buf->status_bar.texture);
    SDL_SetRenderDrawColor(renderer, buf->status_bar.text_color.r, buf->status_bar.text_color.g, buf->status_bar.text_color.b, buf->status_bar.text_color.a);
    
    SDL_SetRenderDrawColor(renderer, buf->status_bar.color.r, buf->status_bar.color.g, buf->status_bar.color.b, buf->status_bar.color.a);
    SDL_Rect clear_rect = {1, 1, buf->status_bar.w - 2, buf->status_bar.h - 2};
    SDL_RenderFillRect(app.renderer, &clear_rect);
    
    int cur_char;
    SDL_Rect glyph_rect;
    SDL_Rect pos;
    
    int len_fn = strlen(buf->filename);
    for (int i = 0; i < len_fn; ++i)
    {
        cur_char = (int)buf->filename[i];
        glyph_rect = {(cur_char - 32) * font.width, 0, font.width, font.height};
        pos = {margin + (i * font.width), margin, font.width, font.height};
        SDL_RenderCopy(app.renderer, bar_characters_texture, &glyph_rect, &pos);
    }
    
    // print line/col
    int len_lc = GetDigitCount(buf->line) + GetDigitCount(buf->column) + 4;
    char linecol_str[20] = {0};
    sprintf (linecol_str, "L%d,C%d", buf->line + 1, buf->column + 1);
    
    for (int i = 0; i < len_lc; ++i)
    {
        cur_char = linecol_str[i];
        
        glyph_rect = {(cur_char - 32) * font.width, 0, font.width, font.height};
        pos = {((len_fn + 2) * font.width) + margin + (i * font.width), margin, font.width, font.height};
        SDL_RenderCopy(app.renderer, bar_characters_texture, &glyph_rect, &pos);
    }
    
    SDL_SetRenderTarget(app.renderer, NULL);
};

void HighlightLineDraw(SDL_Renderer *renderer, buffer *buf){
    int xx = buf->panel.x + margin;
    int yy = (buf->cursor.row * font.height) + margin;
    
    SDL_Rect box ={xx, yy, buf->panel.w - (2 *margin), font.height};
    SDL_SetRenderDrawColor(renderer, buf->cursor.line_highlight.r, buf->cursor.line_highlight.g, buf->cursor.line_highlight.b, buf->cursor.line_highlight.a);
    SDL_RenderFillRect(renderer, &box);
};

void WindowResize(app_info *application, SDL_Window *win)
{
    SDL_GetWindowSize(win, &application->ww, &application->wh);
    
    bufferA.status_bar.h = font.height + margin;
    bufferA.panel.x = 1;
    bufferA.panel.y = 1;
    bufferA.panel.w = (application->ww / 2) - 1;
    bufferA.panel.h = application->wh - bufferA.status_bar.h - 1;
    bufferA.panel.row_capacity = bufferA.panel.h / font.height;
    bufferA.panel.col_capacity = bufferA.panel.w / font.width;
    bufferA.status_bar.x = bufferA.panel.x;
    bufferA.status_bar.y = bufferA.panel.h;
    bufferA.status_bar.w = bufferA.panel.w;
    
    bufferB.status_bar.h = font.height + margin;
    bufferB.panel.x = (application->ww / 2) + 1;
    bufferB.panel.y = 1;
    bufferB.panel.w = (application->ww / 2) - 1;
    bufferB.panel.h = application->wh - bufferB.status_bar.h - 1;
    bufferB.panel.row_capacity = bufferB.panel.h / font.height;
    bufferB.panel.col_capacity = bufferB.panel.w / font.width;
    bufferB.status_bar.x = bufferB.panel.x;
    bufferB.status_bar.y = bufferB.panel.h;
    bufferB.status_bar.w = bufferB.panel.w;
};

void RenderCharacterAt(buffer *buf, node *line_node, int row, int col, int row_length, SDL_Texture *ch, SDL_Texture *pt)
{
    SDL_SetRenderTarget(app.renderer, pt);
    int cur_char = (int)app.e.text.text[0];
    SDL_Rect glyph_rect = {(cur_char - 32) * font.width, 0, font.width, font.height};
    
    if(col == row_length - 1) // draw last character in line
    {
        SDL_Rect pos = {margin + (col * font.width) - (buf->panel.page * buf->panel.col_capacity * font.width), margin + (row * font.height), font.width, font.height};
        SDL_RenderCopy(app.renderer, ch, &glyph_rect, &pos);
    }
    else //draw current character & trailing characters
    {
        int chars_to_clear = row_length - col;
        SDL_Rect clear_rect = {margin + (col * font.width) - (buf->panel.page * buf->panel.col_capacity * font.width), margin + (row * font.height), chars_to_clear * font.width, font.height};
        SDL_RenderFillRect(app.renderer, &clear_rect);
        
        for (int i = col; i < row_length; ++i)
        {
            int cur_char = (int)line_node->data[i];
            SDL_Rect glyph_rect = {(cur_char - 32) * font.width, 0, font.width, font.height};
            SDL_Rect pos = {margin + (i * font.width) - (buf->panel.page * buf->panel.col_capacity * font.width), margin + (buf->cursor.row * font.height), font.width, font.height};
            
            SDL_RenderCopy(app.renderer, ch, &glyph_rect, &pos);
        }
    }
    
    SDL_SetRenderTarget(app.renderer, NULL);
};

void RenderClearCharacterAt(buffer *buf, node *line_node, int row, int col, int row_length, SDL_Texture *ch, SDL_Texture *pt)
{
    SDL_SetRenderTarget(app.renderer, pt);
    SDL_SetRenderDrawColor(app.renderer, 21, 12, 42, 0);// background
    int cur_char = (int)line_node->data[col];
    int chars_to_clear = row_length + 1 - col;
    SDL_Rect glyph_rect = {(cur_char - 32) * font.width, 0, font.width, font.height};
    
    SDL_Rect clear_rect = {margin + (col * font.width) - (buf->panel.page * buf->panel.col_capacity * font.width), margin + (row * font.height), chars_to_clear * font.width, font.height};
    SDL_RenderFillRect(app.renderer, &clear_rect);
    
    
    if(col < row_length)
    {
        for (int i = col; i < row_length; ++i)
        {
            int cur_char = (int)line_node->data[i];
            glyph_rect = {(cur_char - 32) * font.width, 0, font.width, font.height};
            SDL_Rect pos = {margin + (i * font.width) - (buf->panel.page * buf->panel.col_capacity * font.width), margin + (buf->cursor.row * font.height), font.width, font.height};
            
            SDL_RenderCopy(app.renderer, ch, &glyph_rect, &pos);
        }
    }
    SDL_SetRenderTarget(app.renderer, NULL);
};

void RenderClearLine(buffer *buf, node *line_node, int row, SDL_Texture *ch, SDL_Texture *pt)
{
    SDL_SetRenderTarget(app.renderer, pt);
    SDL_SetRenderDrawColor(app.renderer, 21, 12, 42, 0);// background
    
    int cur_char = (int)line_node->data[0];
    
    SDL_Rect glyph_rect = {(cur_char - 32) * font.width, 0, font.width, font.height};
    
    SDL_Rect clear_rect = {margin, margin + (row * font.height), buf->panel.w, (buf->line_count + 1) * font.height};
    SDL_RenderFillRect(app.renderer, &clear_rect);
    
    for (int i = row; i < buf->line_count; ++i)
    {
        int start = (buf->panel.page * buf->panel.col_capacity);
        for (int j = start; j < strlen(line_node->data); ++j)
        {
            cur_char = (int)line_node->data[j];
            SDL_Rect glyph_rect = {(cur_char - 32) * font.width, 0, font.width, font.height};
            SDL_Rect pos = {margin + ((j - start) * font.width), margin + (i * font.height), font.width, font.height};
            SDL_RenderCopy(app.renderer, ch, &glyph_rect, &pos);
        }
        
        if(line_node->next != NULL)
        {
            line_node = line_node->next;
        }
        
    }
    
    SDL_SetRenderTarget(app.renderer, NULL);
};


void RenderLineRange(buffer *buf, int start, int count, SDL_Texture *ch, SDL_Texture *pt)
{
    SDL_SetRenderTarget(app.renderer, pt);
    SDL_SetRenderDrawColor(app.renderer, 21, 12, 42, 0);// background
    SDL_RenderFillRect(app.renderer, NULL);
    
    SDL_Rect glyph_rect;
    SDL_Rect pos_rect;
    
    node *cur_node = buf->head;
    
    for (int i = 0; i <= start; ++i)
    {
        cur_node = cur_node->next;
    }
    
    for (int i = 0; i < count; ++i)
    {
        int start = (buf->panel.page * buf->panel.col_capacity);
        int len = strlen(cur_node->data);
        
        for (int j = start; j < len; ++j)
        {
            int cur_char = (int)cur_node->data[j];
            
            glyph_rect = {(cur_char - 32) * font.width, 0, font.width, font.height};
            pos_rect = {margin + ((j - start) * font.width), margin + (i * font.height), font.width, font.height};
            
            SDL_RenderCopy(app.renderer, ch, &glyph_rect, &pos_rect);
        }
        
        cur_node = cur_node->next;
    }
    
    SDL_SetRenderTarget(app.renderer, NULL);
};


void SyncCursorWithBuffer(buffer *buf)
{
    buf->cursor.row = buf->line - buf->panel.scroll_offset_ver;
    buf->cursor.col = buf->column;
};

void SwitchHorizontalPage(buffer *buf)
{
    //page advance test
    if(buf->column + 1 > (buf->panel.page + 1) * buf->panel.col_capacity)
    {
        buf->panel.page++;
        RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
    }
    if(buf->column  < buf->panel.page * buf->panel.col_capacity)
    {
        buf->panel.page--;
        RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
    }
};