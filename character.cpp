void InsertCharacterAt(buffer *buf, node *row, int col)
{
    U8_strinsert(row->data, buf->cursor.column, app.e.text.text, 128);
    buf->cursor.column++;
};

node *DeleteCharacterAt(buffer *buf, node *row, int col)
{
    //NOTE:if there are trailing characters AFTER the deleted character,
    // and if cursor column is about to jump to previous row,
    // move all trailing characters to the previous line!
    
    if(buf->cursor.column > 0)
    {
        buf->cursor.column--;
        U8_strdel(row->data, buf->cursor.column);
        //RenderCharacterAt(row, buf->cursor.line, buf->cursor.column, strlen(row->data), characters_texture, im_texture);
        
        //SDL_SetRenderTarget(app.renderer, im_texture);
        //SDL_SetRenderDrawColor(app.renderer, 21, 12, 42, 0);
        //node *current = headA->next;
        
        //NOTE:hardcoded this rect's width for now
        //SDL_Rect char_rect = {4+bufferA.cursor.column*font.width, 4+bufferA.cursor.line*font.height, 150,font.height};
        //SDL_RenderFillRect(app.renderer, &char_rect);
        
        //for (int j = 0; j < 128; ++j)
        //{
        //int cur_char_code = (int)a->data[j];
        //SDL_Rect glyph_rect = {(cur_char_code - 32)*font.width,0,font.width,font.height};
        //SDL_Rect pos = {4+(j*font.width),4+(bufferA.cursor.line*font.height),font.width,font.height};
        //
        //SDL_RenderCopy(app.renderer, characters_texture, &glyph_rect, &pos);
        //
        //}
        //
        //SDL_SetRenderTarget(app.renderer, NULL);
        return row;
    }
    else
    {
        if(row->prev != buf->head)
        {
            DeleteLineAt(buf, buf->cursor.line);
            row = row->prev;
            buf->cursor.line--;
            buf->cursor.column = strlen(row->data);
            
            return row;
        }
        else
        {
            return buf->head->next;
        }
    }
};