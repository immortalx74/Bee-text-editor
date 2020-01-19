//NOTE: need to set length and shrink strings on
// Trim functions!!!!!!!!!!!!!!!!!
2
3
1234  78
012345

//settings ideas
font_name = liberation-mono.ttf
font_size = 14
start_path = app_path
tab_size = 4 (limit to 1 - 32)
color_background = 21, 12, 42, 255
color_panel_outline = 100, 100, 100, 255
color_text = 143, 175, 127, 255
color_line_highlight = 40, 0, 180, 255
color_cursor = 0, 255, 0, 255
color_marker = 255, 0, 0, 255
color_bar_background = 140, 140, 140, 255
color_bar_text = 10, 10, 10, 255

asdf


color_panel_outline = 100, 100, 100, 255
color_text = 143, 175, 127, 255
color_line_highlight = 40, 0, 180, 255

jhjhkjhhkkhk


// MULTILINE:
// First line ALWAYS stays
// ALL other lines get deleted
// If there's any content left on last line,
// it gets merged with first
// (check line capacity first!)



500
100 w400

1234qwerty

void A_WeaponReady();
void A_Raise();
void A_ReFire();
void A_Light1();
void A_Light2();
void A_CheckReload();adShotgun2();
void A_LoadShotgun2();
void A_FireCGun();
void A_FireMissile();
void A_Saw();
void A_FirePlasma();
void A_BFGsound();
void A_FireBFG();
void A_BFGSpray();
void A_Explode();

q0wertyuio

0123456789

oooooooooo
1111111111

sassss,wewewee1234,eweeee,bbvbbbb.1321
123   7890
000456




qwert


qwertyuop
1234567890


1234567890


//undostackstoreop
if(t == OP_INSERT)
{
    if(undo_rec_index >= 0)
    {
        int last_char_index = strlen(undo_stack[undo_rec_index].text->data) - 1;
        int len = XstringGetLength(undo_stack[undo_rec_index].text);
        
        if(!undo_stack[undo_rec_index - 1].locked && col ==  len && buf->line == row && IsCharacterAlphaNumeric(text[0]) && IsCharacterAlphaNumeric(undo_stack[undo_rec_index].text->data[last_char_index]))// keep adding chars to this OP
        {
            XstringConcat(undo_stack[undo_rec_index].text, 1, text);
        }
        else// break previous OP and add a new one
        {
            undo_stack[undo_rec_index].locked = true;
            
            xstring *txt = XstringCreate(text);
            text_op op = {buf, t, row, col, false, txt};
            
            undo_stack[undo_rec_index].col++;
            undo_rec_index++;
            undo_stack[undo_rec_index] = op;
        }
    }
    else// first OP added to the stack
    {
        xstring *txt = XstringCreate(text);
        text_op op = {buf, t, row, col, false, txt};
        
        undo_stack[undo_rec_index].col++;
        undo_rec_index++;
        undo_stack[undo_rec_index] = op;
    }
}

qwerty123zxcvbnm

qwerty,u
qwertyuiop123456
qwp12345600000000





//CLIPBOARD
void ClipBoardCopy(buffer *buf)
{
    if(buf->column == buf->marker.col && buf->line == buf->marker.row)
    {
        return;
    }
    
    int start_line = MIN(buf->line, buf->marker.row);
    int end_line = MAX(buf->line, buf->marker.row);
    int line_diff = end_line - start_line;
    int start_col, end_col;
    
    if(buf->line != buf->marker.row)
    {
        if(start_line == buf->line)
        {
            start_col = buf->column;
            end_col = buf->marker.col;
        }
        else
        {
            start_col = buf->marker.col;
            end_col = buf->column;
        }
    }
    else
    {
        if(buf->column > buf->marker.col)
        {
            start_col = buf->marker.col;
            end_col = buf->column;
        }
        else if(buf->column < buf->marker.col)
        {
            start_col = buf->column;
            end_col = buf->marker.col;
        }
    }
    
    XstringDestroy(clipboard.text);
    clipboard.text = XstringCreate("");
    
    if(buf->line != buf->marker.row)//multi line
    {
        node *cur = buf->line_node;
        
        if(buf->line > buf->marker.row)
        {
            // Firstly move to the node where the marker is at
            for (int i = 0; i < line_diff; ++i)
            {
                cur = cur->prev;
            }
        }
        
        for (int i = 0; i <= line_diff; ++i)
        {
            int s = 0;
            int e = strlen(cur->data);
            
            if(i == 0)
            {
                s = start_col;
            }
            if(i == line_diff)
            {
                e = end_col;
            }
            
            for (int j = s; j < e; ++j)
            {
                char *ch = (char*)calloc(2,1);
                ch[0] = cur->data[j];
                XstringConcat(clipboard.text, 1, ch);
                free(ch);
            }
            
            if(i != line_diff)
            {
                XstringConcat(clipboard.text, 1, "\r\n");
            }
            
            cur = cur->next;
        }
    }
    else//single line
    {
        for (int i = start_col; i < end_col; ++i)
        {
            char *ch = (char*)calloc(2,1);
            ch[0] = buf->line_node->data[i];
            XstringConcat(clipboard.text, 1, ch);
            free(ch);
        }
    }
    
    SDL_SetClipboardText(XstringGet(clipboard.text));
};

void ClipBoardCut(buffer *buf)
{
    if(buf->column == buf->marker.col && buf->line == buf->marker.row)
    {
        return;
    }
    
    int start_line = MIN(buf->line, buf->marker.row);
    int end_line = MAX(buf->line, buf->marker.row);
    int line_diff = end_line - start_line;
    int start_col, end_col;
    
    if(buf->line != buf->marker.row)
    {
        if(start_line == buf->line)
        {
            start_col = buf->column;
            end_col = buf->marker.col;
        }
        else
        {
            start_col = buf->marker.col;
            end_col = buf->column;
        }
    }
    else
    {
        if(buf->column > buf->marker.col)
        {
            start_col = buf->marker.col;
            end_col = buf->column;
        }
        else if(buf->column < buf->marker.col)
        {
            start_col = buf->column;
            end_col = buf->marker.col;
        }
    }
    
    XstringDestroy(clipboard.text);
    clipboard.text = XstringCreate("");
    
    if(buf->line != buf->marker.row)//multi line
    {
        xstring *merged = XstringCreate("");
        node *cur = buf->line_node;
        node *ref = cur;
        
        if(buf->line > buf->marker.row)
        {
            // Firstly move to the node where the marker is at
            for (int i = 0; i < line_diff; ++i)
            {
                cur = cur->prev;
            }
            
            ref = cur;
        }
        
        for (int i = 0; i <= line_diff; ++i)
        {
            int s = 0;
            int e = strlen(cur->data);
            
            if(i == 0)
            {
                s = start_col;
                
                // Copy left of start line to merged
                for (int i = 0; i < start_col; ++i)
                {
                    char *ch = (char*)calloc(2,1);
                    ch[0] = cur->data[i];
                    XstringConcat(merged, 1, ch);
                    free(ch);
                }
            }
            if(i == line_diff)
            {
                e = end_col;
                
                // Copy right of end line to merged
                int len = strlen(cur->data);
                
                for (int i = e; i < len; ++i)
                {
                    char *ch = (char*)calloc(2,1);
                    ch[0] = cur->data[i];
                    XstringConcat(merged, 1, ch);
                    free(ch);
                }
            }
            
            for (int j = s; j < e; ++j)
            {
                char *ch = (char*)calloc(2,1);
                ch[0] = cur->data[j];
                XstringConcat(clipboard.text, 1, ch);
                free(ch);
            }
            
            if(i != line_diff)
            {
                XstringConcat(clipboard.text, 1, "\r\n");
            }
            
            cur = cur->next;
        }
        
        // Delete lines
        for (int i = end_line; i >= start_line ; --i)
        {
            LineDelete(buf, i);
        }
        
        // Insert merged line
        LineInsert(buf, start_line);
        int chunks = (merged->length / settings.line_mem_chunk) + 1;
        
        if(buf->line_node->num_chunks < chunks)
        {
            LineRequestMemChunks(buf->line_node, chunks);
        }
        
        memcpy(buf->line_node->data, merged->data, merged->length);
        XstringDestroy(merged);
        buf->line = start_line;
        buf->column = start_col;
    }
    else//single line
    {
        int len = strlen(buf->line_node->data);
        xstring *merged = XstringCreate("");
        
        for (int i = 0; i < len; ++i)
        {
            if(i >= start_col && i < end_col)
            {
                char *ch = (char*)calloc(2,1);
                ch[0] = buf->line_node->data[i];
                XstringConcat(clipboard.text, 1, ch);
                free(ch);
            }
            else if(start_col > 0 || end_col < len)//Copy leftovers(if any)
            {
                char *ch = (char*)calloc(2,1);
                ch[0] = buf->line_node->data[i];
                XstringConcat(merged, 1, ch);
                free(ch);
            }
            
            buf->line_node->data[i] = 0;
        }
        
        int merged_len = merged->length;
        
        for (int i = 0; i < merged_len; ++i)
        {
            buf->line_node->data[i] = merged->data[i];
        }
        
        XstringDestroy(merged);
        LineShrinkMemChunks(buf->line_node);
    }
    
    SDL_SetClipboardText(XstringGet(clipboard.text));
    
    buf->marker.row = buf->line;
    buf->marker.col = buf->column;
    SyncCursorWithBuffer(buf);
    
    RenderLineRange(buf, buf->panel.scroll_offset_ver, buf->panel.row_capacity, characters_texture, buf->panel.texture);
};
