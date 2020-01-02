#include "file.h"
#include <iostream>
#include "draw.h"

void FileReadToBuffer(buffer *buf, char *filename)
{
    std::ifstream file;
    file.open(filename);
    std::string str;
    str.reserve(LINE_MEM_CHUNK);
    
    node *cur_node = KillBuffer(buf);
    
    while (std::getline(file, str))
    {
        if(str.length() / LINE_MEM_CHUNK > 0)
        {
            int num = (str.length() / LINE_MEM_CHUNK) + 1;
            LineRequestMemChunks(cur_node, num);
        }
        
        strcpy(cur_node->data, str.c_str());
        buf->column = strlen(cur_node->data);
        
        node *newline = (node*)malloc(sizeof(node));
        newline->data = (char*)calloc(LINE_MEM_CHUNK, 1);
        newline->num_chunks = 1;
        newline->prev = cur_node;
        newline->next = NULL;
        cur_node->next = newline;
        cur_node = newline;
        buf->line_count++;
    }
    
    //TEMP hack. Delete that extra last line
    DeleteLineAt(buf, buf->line_count - 1);
    
    file.close();
    
    //Set cursor to first line
    cur_node = buf->head->next;
    
    buf->line = 0;
    buf->column = 0;
    buf->cursor.row = 0;
    buf->cursor.col = 0;
    buf->cursor.last_hor_pos = 0;
    buf->panel.scroll_offset_ver = 0;
    buf->panel.page = 0;
    buf->line_node = cur_node;
    
    
    //TEMP set filename
    int len = strlen(buf->filename);
    for (int i = 0; i < len; ++i)
    {
        buf->filename[i] = 0;
    }
    
    len = strlen(filename);
    for (int i = 0; i < len; ++i)
    {
        buf->filename[i] = filename[i];
    }
    
    // Render content
    RenderLineRange(buf, 0, buf->line_count, characters_texture, buf->panel.texture);
};

void FileWriteToDisk(buffer *buf, char *filename)
{
    std::ofstream file;
    file.open(filename);
    
    node *cur = buf->head->next;
    
    while(cur != NULL)
    {
        if (!file)
        {
            std::cout << "Somthing failed while opening the file\n";
            SDL_Delay(1000);
        }
        file << cur->data;
        file << "\n";
        cur = cur->next;
    }
    
    file.close();
};

void FileParseSettings()
{
    const int token_count = 12;
    xstring *tokens[token_count];
    tokens[0] = XstringCreate("font_name");
    tokens[1] = XstringCreate("font_size");
    tokens[2] = XstringCreate("start_path");
    tokens[3] = XstringCreate("tab_size");
    tokens[4] = XstringCreate("color_background");
    tokens[5] = XstringCreate("color_panel_outline");
    tokens[6] = XstringCreate("color_text");
    tokens[7] = XstringCreate("color_line_highlight");
    tokens[8] = XstringCreate("color_cursor");
    tokens[9] = XstringCreate("color_marker");
    tokens[10] = XstringCreate("color_bar_background");
    tokens[11] = XstringCreate("color_bar_text");
    
    
    std::ifstream file;
    file.open("settings.cfg");
    std::string str;
    xstring *line = XstringCreate("");
    
    xstring *identifier = XstringCreate("");
    xstring *value = XstringCreate("");
    
    while(std::getline(file, str))
    {
        char *s = (char*)malloc(str.length() + 1);
        memcpy(s, str.c_str(), str.size());
        s[str.size()] = 0;
        XstringSet(line, s);
        free(s);
        XstringTrimLeadingAndTrailingWhitespace(line);
        
        // Ignore commented lines
        if(XstringGetLength(line) >= 2 && XstringGet(line)[0] != '/' && XstringGet(line)[1] != '/')
        {
            for (int i = 0; i < token_count; ++i)
            {
                if(XstringContainsSubstring(line, XstringGet(tokens[i])))
                {
                    //Token found. Get value
                    XstringSet(identifier, XstringGet(tokens[i]));
                    XstringTruncateHead(line, XstringGetLength(identifier));
                    
                    if(XstringGet(line)[0] == ' ')// At least 1 whitespace after identifier
                    {
                        XstringTrimLeadingAndTrailingWhitespace(line);
                        XstringSet(value, line->data);
                        //handle cases depending on token index
                        SetSetting(i, value);
                    }
                    else
                    {
                        //invalid
                    }
                }
            }
        }
    }
    
    XstringDestroy(identifier);
    XstringDestroy(value);
    XstringDestroy(line);
    for (int i = 0; i < token_count; ++i)
    {
        XstringDestroy(tokens[i]);
    }
    file.close();
};

void SetSetting(int index, xstring *value)
{
    switch (index)
    {
        case 0://font_name
        {
            XstringTruncateHead(value, 1);
            XstringTruncateTail(value, 1);
            std::ifstream f(value->data);
            if(f.good())
            {
                XstringSet(settings.font_name, value->data);
            }
        }
        break;
        
        case 1://font_size
        {
            settings.font_size = atoi(value->data);
        }
        break;
        
        case 2://start_path
        {
            XstringTruncateHead(value, 1);
            XstringTruncateTail(value, 1);
            XstringSet(settings.start_path, value->data);
        }
        break;
        
        case 3://tab_size
        {
            settings.tab_size = atoi(value->data);
        }
        break;
        
        case 4://color_background
        {
            XstringTrimAllWhitespace(value);
            XstringTruncateHead(value, 1);
            XstringTruncateTail(value, 1);
            settings.color_background = ExtractColorFromString(value);
        }
        break;
        
        case 5://color_panel_outline
        {
            XstringTrimAllWhitespace(value);
            XstringTruncateHead(value, 1);
            XstringTruncateTail(value, 1);
            settings.color_panel_outline = ExtractColorFromString(value);
        }
        break;
        
        case 6://color_text
        {
            XstringTrimAllWhitespace(value);
            XstringTruncateHead(value, 1);
            XstringTruncateTail(value, 1);
            settings.color_text = ExtractColorFromString(value);
        }
        break;
        
        case 7://color_line_highlight
        {
            XstringTrimAllWhitespace(value);
            XstringTruncateHead(value, 1);
            XstringTruncateTail(value, 1);
            settings.color_line_highlight = ExtractColorFromString(value);
        }
        break;
        
        case 8://color_cursor
        {
            XstringTrimAllWhitespace(value);
            XstringTruncateHead(value, 1);
            XstringTruncateTail(value, 1);
            settings.color_cursor = ExtractColorFromString(value);
        }
        break;
        
        case 9://color_marker
        {
            XstringTrimAllWhitespace(value);
            XstringTruncateHead(value, 1);
            XstringTruncateTail(value, 1);
            settings.color_marker = ExtractColorFromString(value);
        }
        break;
        
        case 10://color_bar_background
        {
            XstringTrimAllWhitespace(value);
            XstringTruncateHead(value, 1);
            XstringTruncateTail(value, 1);
            settings.color_bar_background = ExtractColorFromString(value);
        }
        break;
        
        case 11://color_bar_text
        {
            XstringTrimAllWhitespace(value);
            XstringTruncateHead(value, 1);
            XstringTruncateTail(value, 1);
            settings.color_bar_text = ExtractColorFromString(value);
        }
        break;
    }
};

SDL_Color ExtractColorFromString(xstring *str)
{
    int len = str->length;
    int index = 0;
    char *ch = (char*)calloc(2, 1);
    Uint8 component[4];
    xstring *str_comp[4] = {XstringCreate(""), XstringCreate(""), XstringCreate(""), XstringCreate("")};
    
    for (int i = 0; i < len; ++i)
    {
        ch[0] = str->data[i];
        
        if(IsCharacterNumeric(ch[0]))
        {
            XstringConcat(str_comp[index], 1, ch);
        }
        else if(ch[0] == ',')
        {
            index++;
        }
    }
    
    for (int i = 0; i < 4; ++i)
    {
        component[i] = atoi(str_comp[i]->data);
        XstringDestroy(str_comp[i]);
    }
    
    SDL_Color result = {component[0], component[1], component[2], component[3]};
    free(ch);
    
    return result;
};