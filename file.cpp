#include "file.h"

void FileReadToBuffer(buffer *buf, char *filename)
{
    std::ifstream file;
    file.open(filename);
    std::string str;
    str.reserve(settings.line_mem_chunk);
    
    node *cur_node = KillBuffer(buf);
    
    while (std::getline(file, str))
    {
        if(str.length() / settings.line_mem_chunk > 0)
        {
            int num = (str.length() / settings.line_mem_chunk) + 1;
            LineRequestMemChunks(cur_node, num);
        }
        
        strcpy(cur_node->data, str.c_str());
        buf->column = strlen(cur_node->data);
        
        node *newline = (node*)malloc(sizeof(node));
        newline->data = (char*)calloc(settings.line_mem_chunk, 1);
        newline->num_chunks = 1;
        newline->prev = cur_node;
        newline->next = NULL;
        cur_node->next = newline;
        cur_node = newline;
        buf->line_count++;
    }
    
    //TEMP hack. Delete that extra last line
    LineDelete(buf, buf->line_count - 1);
    
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
    
    XstringSet(buf->filename, filename);
    
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
    std::ifstream file;
    
    //std::ifstream test("settings.cfg");
    //if(!test.good())
    //{
    //file not found
    //std::cout << "file not found" << std::endl;
    //return;
    //}
    
    const int sets_count = 23;// Change when adding/removing a setting
    xstring *settings_tokens[sets_count];
    
    settings_tokens[sets_enum::font_name] = XstringCreate("font_name");
    settings_tokens[sets_enum::start_path] = XstringCreate("start_path");
    settings_tokens[sets_enum::font_size] = XstringCreate("font_size");
    settings_tokens[sets_enum::tab_size] = XstringCreate("tab_size");
    settings_tokens[sets_enum::line_mem_chunk] = XstringCreate("line_mem_chunk");
    settings_tokens[sets_enum::margin] = XstringCreate("margin");
    settings_tokens[sets_enum::undo_steps] = XstringCreate("undo_steps");
    settings_tokens[sets_enum::cursor_blink] = XstringCreate("cursor_blink");
    settings_tokens[sets_enum::cursor_blink_rate] = XstringCreate("cursor_blink_rate");
    settings_tokens[sets_enum::draw_line_highlight] = XstringCreate("draw_line_highlight");
    settings_tokens[sets_enum::color_background] = XstringCreate("color_background");
    settings_tokens[sets_enum::color_panel_outline] = XstringCreate("color_panel_outline");
    settings_tokens[sets_enum::color_text] = XstringCreate("color_text");
    settings_tokens[sets_enum::color_line_highlight] = XstringCreate("color_line_highlight");
    settings_tokens[sets_enum::color_cursor] = XstringCreate("color_cursor");
    settings_tokens[sets_enum::color_marker] = XstringCreate("color_marker");
    settings_tokens[sets_enum::color_bar_background] = XstringCreate("color_bar_background");
    settings_tokens[sets_enum::color_bar_text] = XstringCreate("color_bar_text");
    
    settings_tokens[sets_enum::command_file_open] = XstringCreate("command_file_open");
    settings_tokens[sets_enum::command_file_save] = XstringCreate("command_file_save");
    settings_tokens[sets_enum::command_marker_set] = XstringCreate("command_marker_set");
    settings_tokens[sets_enum::command_buffer_kill] = XstringCreate("command_buffer_kill");
    settings_tokens[sets_enum::command_buffer_toggle_active] = XstringCreate("command_buffer_toggle_active");
    
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
            for (int i = 0; i < sets_count; ++i)
            {
                if(XstringContainsSubstring(line, XstringGet(settings_tokens[i])))
                {
                    int len = settings_tokens[i]->length;
                    if(line->data[len] == ' ')
                    {
                        //Token found. Get value
                        XstringSet(identifier, XstringGet(settings_tokens[i]));
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
    }
    
    XstringDestroy(identifier);
    XstringDestroy(value);
    XstringDestroy(line);
    for (int i = 0; i < sets_count; ++i)
    {
        XstringDestroy(settings_tokens[i]);
    }
    file.close();
    
    SettingsApply();
};

void SetSetting(int index, xstring *value)
{
    switch (index)
    {
        case sets_enum::font_name:
        {
            XstringTruncateHead(value, 1);
            XstringTruncateTail(value, 1);
            XstringSet(settings.font_name, value->data);
        }
        break;
        
        case sets_enum::font_size:
        {
            XstringTrimAllWhitespace(value);
            settings.font_size = atoi(value->data);
        }
        break;
        
        case sets_enum::start_path:
        {
            XstringTruncateHead(value, 1);
            XstringTruncateTail(value, 1);
            if(value->length == 1)
            {
                XstringSet(settings.start_path, "");
            }
            else
            {
                XstringSet(settings.start_path, value->data);
            }
        }
        break;
        
        case sets_enum::tab_size:
        {
            settings.tab_size = atoi(value->data);
        }
        break;
        
        case sets_enum::line_mem_chunk:
        {
            settings.line_mem_chunk = atoi(value->data);
        }
        break;
        
        case sets_enum::margin:
        {
            settings.margin = atoi(value->data);
        }
        break;
        
        case sets_enum::undo_steps:
        {
            settings.undo_steps = atoi(value->data);
        }
        break;
        
        case sets_enum::cursor_blink:
        {
            settings.cursor_blink = atoi(value->data);
        }
        break;
        
        case sets_enum::cursor_blink_rate:
        {
            settings.cursor_blink_rate = atoi(value->data);
        }
        break;
        
        case sets_enum::draw_line_highlight:
        {
            settings.draw_line_highlight = atoi(value->data);
        }
        break;
        
        case sets_enum::color_background:
        {
            XstringTrimAllWhitespace(value);
            XstringTruncateHead(value, 1);
            XstringTruncateTail(value, 1);
            settings.color_background = ExtractColorFromString(value);
        }
        break;
        
        case sets_enum::color_panel_outline:
        {
            XstringTrimAllWhitespace(value);
            XstringTruncateHead(value, 1);
            XstringTruncateTail(value, 1);
            settings.color_panel_outline = ExtractColorFromString(value);
        }
        break;
        
        case sets_enum::color_text:
        {
            XstringTrimAllWhitespace(value);
            XstringTruncateHead(value, 1);
            XstringTruncateTail(value, 1);
            settings.color_text = ExtractColorFromString(value);
        }
        break;
        
        case sets_enum::color_line_highlight:
        {
            XstringTrimAllWhitespace(value);
            XstringTruncateHead(value, 1);
            XstringTruncateTail(value, 1);
            settings.color_line_highlight = ExtractColorFromString(value);
        }
        break;
        
        case sets_enum::color_cursor:
        {
            XstringTrimAllWhitespace(value);
            XstringTruncateHead(value, 1);
            XstringTruncateTail(value, 1);
            settings.color_cursor = ExtractColorFromString(value);
        }
        break;
        
        case sets_enum::color_marker:
        {
            XstringTrimAllWhitespace(value);
            XstringTruncateHead(value, 1);
            XstringTruncateTail(value, 1);
            settings.color_marker = ExtractColorFromString(value);
        }
        break;
        
        case sets_enum::color_bar_background:
        {
            XstringTrimAllWhitespace(value);
            XstringTruncateHead(value, 1);
            XstringTruncateTail(value, 1);
            settings.color_bar_background = ExtractColorFromString(value);
        }
        break;
        
        case sets_enum::color_bar_text:
        {
            XstringTrimAllWhitespace(value);
            XstringTruncateHead(value, 1);
            XstringTruncateTail(value, 1);
            settings.color_bar_text = ExtractColorFromString(value);
        }
        break;
        
        case sets_enum::command_file_open:
        {
            XstringTrimAllWhitespace(value);
            XstringTruncateHead(value, 1);
            XstringTruncateTail(value, 1);
            settings.kb_command_file_open = ExtractKeyBindingFromString(value);
        }
        break;
        
        case sets_enum::command_file_save:
        {
            XstringTrimAllWhitespace(value);
            XstringTruncateHead(value, 1);
            XstringTruncateTail(value, 1);
            settings.kb_command_file_save = ExtractKeyBindingFromString(value);
        }
        break;
        
        case sets_enum::command_marker_set:
        {
            XstringTrimAllWhitespace(value);
            XstringTruncateHead(value, 1);
            XstringTruncateTail(value, 1);
            settings.kb_command_marker_set = ExtractKeyBindingFromString(value);
        }
        break;
        
        case sets_enum::command_buffer_kill:
        {
            XstringTrimAllWhitespace(value);
            XstringTruncateHead(value, 1);
            XstringTruncateTail(value, 1);
            settings.kb_command_buffer_kill = ExtractKeyBindingFromString(value);
        }
        break;
        
        case sets_enum::command_buffer_toggle_active:
        {
            XstringTrimAllWhitespace(value);
            XstringTruncateHead(value, 1);
            XstringTruncateTail(value, 1);
            settings.kb_command_buffer_toggle_active = ExtractKeyBindingFromString(value);
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

key_binding ExtractKeyBindingFromString(xstring *str)
{
    int len = str->length;
    int index = 0;
    char *ch = (char*)calloc(2, 1);
    xstring *str_keys[3] = {XstringCreate(""), XstringCreate(""), XstringCreate("")};
    key_binding kb;
    
    for (int i = 0; i < len; ++i)
    {
        ch[0] = str->data[i];
        if(ch[0] == ',')
        {
            // special case to exclude ',' character as a key-binding
            if(i < len -1 && str->data[i + 1] != '"')
            {
                index++;
            }
        }
        else
        {
            XstringConcat(str_keys[index], 1, ch);
        }
    }
    
    XstringTruncateTail(str_keys[0], 1);
    XstringTruncateHead(str_keys[0], 1);
    
    SDL_Keycode kc = SDL_GetKeyFromName(str_keys[0]->data);
    kb.key = kc;
    
    int result_mod = GetKeymodFromString(str_keys[1]);
    
    if(result_mod != -1)
    {
        kb.mod1 = result_mod;
    }
    
    result_mod = GetKeymodFromString(str_keys[2]);
    if(result_mod != -1)
    {
        kb.mod2 = result_mod;
    }
    
    for (int i = 0; i < 3; ++i)
    {
        XstringDestroy(str_keys[i]);
    }
    
    return kb;
};

int GetKeymodFromString(xstring * str)
{
    if(XstringContainsSubstring(str, "KMOD_NONE"))
    {
        return KMOD_NONE;
    }
    else if(XstringContainsSubstring(str, "KMOD_LSHIFT"))
    {
        return KMOD_LSHIFT;
    }
    else if(XstringContainsSubstring(str, "KMOD_RSHIFT"))
    {
        return KMOD_RSHIFT;
    }
    else if(XstringContainsSubstring(str, "KMOD_LCTRL"))
    {
        return KMOD_LCTRL;
    }
    else if(XstringContainsSubstring(str, "KMOD_RCTRL"))
    {
        return KMOD_RCTRL;
    }
    else if(XstringContainsSubstring(str, "KMOD_LALT"))
    {
        return KMOD_LALT;
    }
    else if(XstringContainsSubstring(str, "KMOD_RALT"))
    {
        return KMOD_RALT;
    }
    else if(XstringContainsSubstring(str, "KMOD_CTRL"))
    {
        return KMOD_CTRL;
    }
    else if(XstringContainsSubstring(str, "KMOD_SHIFT"))
    {
        return KMOD_SHIFT;
    }
    else if(XstringContainsSubstring(str, "KMOD_ALT"))
    {
        return KMOD_ALT;
    }
    
    return -1;
};