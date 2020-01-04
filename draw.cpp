#include "draw.h"
#include "character.h"
#include <iostream>

void CursorDraw(buffer *buf)
{
    // NOTE xx now takes page into account
    int xx = ((buf->cursor.col - buf->panel.page * buf->panel.col_capacity) * font.width) + settings.margin + buf->panel.x;
    int yy = (buf->cursor.row * font.height) + settings.margin;
    
    SDL_Rect box = {xx, yy, font.width, font.height};
    
    SDL_SetRenderDrawColor(app.renderer, buf->cursor.color.r, buf->cursor.color.g, buf->cursor.color.b, buf->cursor.color.a);
    
    SDL_SetRenderDrawBlendMode(app.renderer, app.custom_blendmode);
    
    SDL_RenderFillRect(app.renderer, &box);
    
    SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(app.renderer, settings.color_background.r, settings.color_background.g, settings.color_background.b, settings.color_background.a);// background
};

void MarkerDraw(buffer *buf)
{
    int xx = (buf->marker.col - (buf->panel.page * buf->panel.col_capacity)) * font.width + settings.margin + buf->panel.x;
    //int yy = (buf->marker.row * font.height) + settings.margin;
    int yy = (buf->marker.row - buf->panel.scroll_offset_ver) * font.height + settings.margin;
    
    SDL_Rect box = {xx, yy, font.width, font.height};
    
    SDL_SetRenderDrawColor(app.renderer, buf->marker.color.r, buf->marker.color.g, buf->marker.color.b, buf->marker.color.a);
    
    SDL_RenderDrawRect(app.renderer, &box);
    
    SDL_SetRenderDrawColor(app.renderer, settings.color_background.r, settings.color_background.g, settings.color_background.b, settings.color_background.a);// background
};

void PanelDraw(buffer *buf)
{
    SDL_Rect box = {buf->panel.x, buf->panel.y, buf->panel.w, buf->panel.h};
    SDL_SetRenderDrawColor(app.renderer, buf->panel.color.r, buf->panel.color.g, buf->panel.color.b, buf->panel.color.a);
    SDL_RenderDrawRect(app.renderer, &box);
};

void BarDraw(buffer *buf)
{
    if(app.mode == TEXT_EDIT)
    {
        SDL_Rect box = {buf->status_bar.x, buf->status_bar.y, buf->status_bar.w, buf->status_bar.h};
        SDL_SetRenderDrawColor(app.renderer, buf->panel.color.r, buf->panel.color.g, buf->panel.color.b, buf->panel.color.a);
        SDL_RenderDrawRect(app.renderer, &box);
        
        //draw info
        SDL_SetRenderTarget(app.renderer, buf->status_bar.texture);
        SDL_SetRenderDrawColor(app.renderer, buf->status_bar.color.r, buf->status_bar.color.g, buf->status_bar.color.b, buf->status_bar.color.a);
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
            pos = {settings.margin + (i * font.width), settings.margin, font.width, font.height};
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
            pos = {((len_fn + 2) * font.width) + settings.margin + (i * font.width), settings.margin, font.width, font.height};
            SDL_RenderCopy(app.renderer, bar_characters_texture, &glyph_rect, &pos);
        }
        
        SDL_SetRenderTarget(app.renderer, NULL);
    }
    else if(app.mode == LIST_NAV)
    {
        SDL_Rect box = {buf->status_bar.x, buf->status_bar.y, buf->status_bar.w, buf->status_bar.h};
        SDL_SetRenderDrawColor(app.renderer, buf->panel.color.r, buf->panel.color.g, buf->panel.color.b, buf->panel.color.a);
        SDL_RenderDrawRect(app.renderer, &box);
        
        SDL_SetRenderTarget(app.renderer, buf->status_bar.texture);
        SDL_SetRenderDrawColor(app.renderer, buf->status_bar.color.r, buf->status_bar.color.g, buf->status_bar.color.b, buf->status_bar.color.a);
        SDL_Rect clear_rect = {1, 1, buf->status_bar.w - 2, buf->status_bar.h - 2};
        SDL_RenderFillRect(app.renderer, &clear_rect);
        
        int cur_char;
        SDL_Rect glyph_rect;
        SDL_Rect pos;
        
        if(buf->lst != NULL)
        {
            int len_title = XstringGetLength(buf->lst->title);
            for (int i = 0; i < len_title; ++i)
            {
                cur_char = (int)XstringGet(buf->lst->title)[i];
                glyph_rect = {(cur_char - 32) * font.width, 0, font.width, font.height};
                pos = {settings.margin + (i * font.width), settings.margin, font.width, font.height};
                SDL_RenderCopy(app.renderer, bar_characters_texture, &glyph_rect, &pos);
            }
            
            int len_cp = XstringGetLength(buf->lst->current_path);
            for (int i = 0; i < len_cp; ++i)
            {
                cur_char = (int)XstringGet(buf->lst->current_path)[i];
                glyph_rect = {(cur_char - 32) * font.width, 0, font.width, font.height};
                pos = {(len_title * font.width) + settings.margin + (i * font.width), settings.margin, font.width, font.height};
                SDL_RenderCopy(app.renderer, bar_characters_texture, &glyph_rect, &pos);
            }
            
            int len_filter = XstringGetLength(buf->lst->filter);
            
            if(len_filter > 0)
            {
                //cur_char = '\\';
                //glyph_rect = {(cur_char - 32) * font.width, 0, font.width, font.height};
                //pos = {(len_title * font.width) + (len_cp * font.width) + settings.margin , settings.margin, font.width, font.height};
                //SDL_RenderCopy(app.renderer, bar_characters_texture, &glyph_rect, &pos);
                
                for (int i = 0; i < len_filter; ++i)
                {
                    cur_char = (int)XstringGet(buf->lst->filter)[i];
                    glyph_rect = {(cur_char - 32) * font.width, 0, font.width, font.height};
                    pos = {(len_title * font.width) + (len_cp * font.width) + settings.margin + (i * font.width), settings.margin, font.width, font.height};
                    SDL_RenderCopy(app.renderer, bar_characters_texture, &glyph_rect, &pos);
                }
            }
        }
        
        
        SDL_SetRenderTarget(app.renderer, NULL);
    }
};

void HighlightLineDraw(buffer *buf)
{
    int xx = buf->panel.x + settings.margin;
    int yy = (buf->cursor.row * font.height) + settings.margin;
    
    SDL_Rect box = {xx, yy, buf->panel.w - (2 *settings.margin), font.height};
    SDL_SetRenderDrawColor(app.renderer, buf->cursor.line_highlight.r, buf->cursor.line_highlight.g, buf->cursor.line_highlight.b, buf->cursor.line_highlight.a);
    
    SDL_RenderFillRect(app.renderer, &box);
    
    SDL_SetRenderDrawColor(app.renderer, settings.color_background.r, settings.color_background.g, settings.color_background.b, settings.color_background.a);// background
};

void HighlightListSelectionDraw(buffer *buf, list *l)
{
    int xx = buf->panel.x + settings.margin;
    int yy = (l->row * font.height) + settings.margin;
    
    SDL_Rect box = {xx, yy, buf->panel.w - (2 *settings.margin), font.height};
    SDL_SetRenderDrawColor(app.renderer, buf->cursor.line_highlight.r, buf->cursor.line_highlight.g, buf->cursor.line_highlight.b, buf->cursor.line_highlight.a);
    SDL_RenderFillRect(app.renderer, &box);
};

void WindowResize(app_info *application, SDL_Window *win)
{
    SDL_GetWindowSize(win, &application->ww, &application->wh);
    
    bufferA.status_bar.h = font.height + settings.margin;
    bufferA.panel.x = 1;
    bufferA.panel.y = 1;
    bufferA.panel.w = (application->ww / 2) - 1;
    bufferA.panel.h = application->wh - bufferA.status_bar.h - 1;
    bufferA.panel.row_capacity = bufferA.panel.h / font.height;
    bufferA.panel.col_capacity = bufferA.panel.w / font.width;
    bufferA.status_bar.x = bufferA.panel.x;
    bufferA.status_bar.y = bufferA.panel.h;
    bufferA.status_bar.w = bufferA.panel.w;
    
    bufferB.status_bar.h = font.height + settings.margin;
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

void RenderCharacterAt(buffer *buf, int row, int col, int row_length, SDL_Texture *ch, SDL_Texture *pt)
{
    SDL_SetRenderTarget(app.renderer, pt);
    int cur_char = (int)app.e.text.text[0];
    SDL_Rect glyph_rect = {(cur_char - 32) * font.width, 0, font.width, font.height};
    
    if(col == row_length - 1) // draw last character in line
    {
        SDL_Rect pos = {settings.margin + (col * font.width) - (buf->panel.page * buf->panel.col_capacity * font.width), settings.margin + (row * font.height), font.width, font.height};
        SDL_RenderCopy(app.renderer, ch, &glyph_rect, &pos);
    }
    else //draw current character & trailing characters
    {
        int chars_to_clear = row_length - col;
        SDL_Rect clear_rect = {settings.margin + (col * font.width) - (buf->panel.page * buf->panel.col_capacity * font.width), settings.margin + (row * font.height), chars_to_clear * font.width, font.height};
        SDL_RenderFillRect(app.renderer, &clear_rect);
        
        for (int i = col; i < row_length; ++i)
        {
            int cur_char = (int)buf->line_node->data[i];
            SDL_Rect glyph_rect = {(cur_char - 32) * font.width, 0, font.width, font.height};
            SDL_Rect pos = {settings.margin + (i * font.width) - (buf->panel.page * buf->panel.col_capacity * font.width), settings.margin + (buf->cursor.row * font.height), font.width, font.height};
            
            SDL_RenderCopy(app.renderer, ch, &glyph_rect, &pos);
        }
    }
    
    SDL_SetRenderTarget(app.renderer, NULL);
};

void RenderClearCharacterAt(buffer *buf, int row, int col, int row_length, SDL_Texture *ch, SDL_Texture *pt)
{
    SDL_SetRenderTarget(app.renderer, pt);
    SDL_SetRenderDrawColor(app.renderer, settings.color_background.r, settings.color_background.g, settings.color_background.b, 0);// background
    int cur_char = (int)buf->line_node->data[col];
    int chars_to_clear = row_length + 1 - col;
    SDL_Rect glyph_rect = {(cur_char - 32) * font.width, 0, font.width, font.height};
    
    SDL_Rect clear_rect = {settings.margin + (col * font.width) - (buf->panel.page * buf->panel.col_capacity * font.width), settings.margin + (row * font.height), chars_to_clear * font.width, font.height};
    SDL_RenderFillRect(app.renderer, &clear_rect);
    
    
    if(col < row_length)
    {
        for (int i = col; i < row_length; ++i)
        {
            int cur_char = (int)buf->line_node->data[i];
            glyph_rect = {(cur_char - 32) * font.width, 0, font.width, font.height};
            SDL_Rect pos = {settings.margin + (i * font.width) - (buf->panel.page * buf->panel.col_capacity * font.width), settings.margin + (buf->cursor.row * font.height), font.width, font.height};
            
            SDL_RenderCopy(app.renderer, ch, &glyph_rect, &pos);
        }
    }
    SDL_SetRenderTarget(app.renderer, NULL);
};

void RenderClearLine(buffer *buf, int row, SDL_Texture *ch, SDL_Texture *pt)
{
    SDL_SetRenderTarget(app.renderer, pt);
    SDL_SetRenderDrawColor(app.renderer, settings.color_background.r, settings.color_background.g, settings.color_background.b, 0);// background
    
    node *cur_node = buf->line_node;
    int cur_char;
    SDL_Rect glyph_rect;
    
    SDL_Rect clear_rect = {settings.margin, settings.margin + (row * font.height), buf->panel.w, (buf->line_count + 1) * font.height};
    SDL_RenderFillRect(app.renderer, &clear_rect);
    
    for (int i = row; i < buf->line_count; ++i)
    {
        int start = (buf->panel.page * buf->panel.col_capacity);
        for (int j = start; j < strlen(cur_node->data); ++j)
        {
            cur_char = (int)cur_node->data[j];
            glyph_rect = {(cur_char - 32) * font.width, 0, font.width, font.height};
            SDL_Rect pos = {settings.margin + ((j - start) * font.width), settings.margin + (i * font.height), font.width, font.height};
            SDL_RenderCopy(app.renderer, ch, &glyph_rect, &pos);
        }
        
        if(cur_node->next != NULL)
        {
            cur_node = cur_node->next;
        }
        
    }
    
    SDL_SetRenderTarget(app.renderer, NULL);
};


void RenderLineRange(buffer *buf, int start, int count, SDL_Texture *ch, SDL_Texture *pt)
{
    SDL_SetRenderTarget(app.renderer, pt);
    SDL_SetRenderDrawColor(app.renderer, settings.color_background.r, settings.color_background.g, settings.color_background.b, 0);// background
    SDL_RenderFillRect(app.renderer, NULL);
    
    SDL_Rect glyph_rect;
    SDL_Rect pos_rect;
    
    node *cur_node = buf->head;
    
    if(count > buf->line_count)
    {
        count = buf->line_count;
    }
    
    for (int i = 0; i <= start; ++i)
    {
        cur_node = cur_node->next;
    }
    
    for (int i = 0; i < count; ++i)
    {
        int page_offset = (buf->panel.page * buf->panel.col_capacity);
        int len = strlen(cur_node->data);
        
        for (int j = page_offset; j < len; ++j)
        {
            int cur_char = (int)cur_node->data[j];
            
            glyph_rect = {(cur_char - 32) * font.width, 0, font.width, font.height};
            pos_rect = {settings.margin + ((j - page_offset) * font.width), settings.margin + (i * font.height), font.width, font.height};
            
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
    if(buf->column + 1 > (buf->panel.page + 1) * buf->panel.col_capacity || buf->column  < buf->panel.page * buf->panel.col_capacity)
    {
        buf->panel.page = buf->column / buf->panel.col_capacity;
        RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
    }
};

bool MarkerIsWithinDrawingBounds(buffer *buf)
{
    if (buf->marker.col / buf->panel.col_capacity == buf->panel.page && buf->marker.row >= buf->panel.scroll_offset_ver && buf->marker.row < buf->panel.scroll_offset_ver + buf->panel.row_capacity)
    {
        return true;
    }
    
    return false;
};

void ListDraw(buffer *buf, list *l, SDL_Texture *ch, SDL_Texture *pt)
{
    SDL_SetRenderTarget(app.renderer, pt);
    SDL_SetRenderDrawColor(app.renderer, settings.color_background.r, settings.color_background.g, settings.color_background.b, 0);// background
    
    int cur_char;
    SDL_Rect glyph_rect;
    
    SDL_RenderFillRect(app.renderer, NULL);
    
    for (int i = 0; i < l->capacity; ++i)
    {
        char *entry = ListGetElement(l, i);
        
        for (int j = 0; j < strlen(entry); ++j)
        {
            cur_char = (int)entry[j];
            SDL_Rect glyph_rect = {(cur_char - 32) * font.width, 0, font.width, font.height};
            SDL_Rect pos = {settings.margin + (j * font.width), settings.margin + (i * font.height), font.width, font.height};
            SDL_RenderCopy(app.renderer, ch, &glyph_rect, &pos);
        }
    }
    
    SDL_SetRenderTarget(app.renderer, NULL);
};

void RenderListRange(buffer *buf, int start, int count, SDL_Texture *ch, SDL_Texture *pt)
{
    SDL_SetRenderTarget(app.renderer, pt);
    SDL_SetRenderDrawColor(app.renderer, settings.color_background.r, settings.color_background.g, settings.color_background.b, 0);// background
    
    int cur_char;
    SDL_Rect glyph_rect;
    
    SDL_RenderFillRect(app.renderer, NULL);
    
    for (int i = 0; i < count; ++i)
    {
        char *entry = ListGetElement(buf->lst, i+start);
        
        for (int j = 0; j < strlen(entry); ++j)
        {
            cur_char = (int)entry[j];
            SDL_Rect glyph_rect = {(cur_char - 32) * font.width, 0, font.width, font.height};
            SDL_Rect pos = {settings.margin + (j * font.width), settings.margin + (i * font.height), font.width, font.height};
            SDL_RenderCopy(app.renderer, ch, &glyph_rect, &pos);
        }
    }
    
    SDL_SetRenderTarget(app.renderer, NULL);
};