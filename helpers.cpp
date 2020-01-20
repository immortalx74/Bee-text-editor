#include "helpers.h"

void SettingsSetDefaults()
{
    app.last_path = XstringCreate("");
    XstringSet(app.last_path, SDL_GetBasePath());
    
    settings.font_name = XstringCreate("liberation-mono.ttf");
    settings.start_path = XstringCreate("");
    settings.margin = 4;
    settings.line_mem_chunk = 64;
    settings.undo_steps = 100;
    settings.cursor_blink = false;
    settings.cursor_blink_rate = 300;
    settings.font_size = 14;
    settings.tab_size = 4;
    settings.color_background = {21, 12, 42, 255};
    settings.color_panel_outline = {100, 100, 100, 255};
    settings.color_text = {143, 175, 127, 255};
    settings.color_line_highlight = {40, 0, 180, 255};
    settings.color_cursor = {0, 255, 0, 255};
    settings.color_marker = {255, 0, 0, 255};
    settings.color_bar_background = {140, 140, 140, 255};
    settings.color_bar_text = {10, 10, 10, 255};
    
    font.name = XstringCreate("liberation-mono.ttf");
    
    settings.kb_command_file_open = {SDLK_o, KMOD_CTRL, KMOD_NONE};
    settings.kb_command_file_save = {SDLK_s, KMOD_CTRL, KMOD_NONE};
    settings.kb_command_marker_set = {SDLK_SPACE, KMOD_CTRL, KMOD_ALT};
    settings.kb_command_buffer_kill = {SDLK_k, KMOD_CTRL, KMOD_ALT};
    settings.kb_command_buffer_toggle_active = {SDLK_0, KMOD_CTRL, KMOD_ALT};
    
    SettingsApply();
};

void SettingsApply()
{
    XstringSet(font.name, settings.font_name->data);
    
    if(XstringGetLength(settings.start_path) > 0)
    {
#ifdef _WIN32
        if(XstringGet(settings.start_path)[XstringGetLength(settings.start_path) - 1] != '\\')
        {
            XstringConcat(settings.start_path, 1, "\\");
        }
#endif
#ifdef __linux__
        if(XstringGet(settings.start_path)[XstringGetLength(settings.start_path) - 1] != '/')
        {
            XstringConcat(settings.start_path, 1, "\\");
        }
#endif
        XstringSet(app.last_path, settings.start_path->data);
    }
    
    font.size = settings.font_size;
    bufferA.panel.color = settings.color_panel_outline;
    bufferB.panel.color = settings.color_panel_outline;
    font.text_color = settings.color_text;
    bufferA.cursor.color = settings.color_cursor;
    bufferB.cursor.color = settings.color_cursor;
    bufferA.cursor.line_highlight = settings.color_line_highlight;
    bufferB.cursor.line_highlight = settings.color_line_highlight;
    bufferA.marker.color = settings.color_marker;
    bufferB.marker.color = settings.color_marker;
    bufferA.status_bar.color = settings.color_bar_background;
    bufferB.status_bar.color = settings.color_bar_background;
    bufferA.status_bar.text_color = settings.color_bar_text;
    bufferB.status_bar.text_color = settings.color_bar_text;
};

bool IsCharacterAlphaNumeric(char ch)
{
    if((ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 122) || (ch >= 48 && ch <= 57))
    {
        return true;
    }
    return false;
};

bool IsCharacterNumeric(char ch)
{
    if(ch >= 48 && ch <= 57)
    {
        return true;
    }
    return false;
};

int GetDigitCount(int n)
{
    if(n == 0)
    {
        return 1;
    }
    
    int count = 0;
    
    while(n != 0)
    {
        n /= 10;
        count++;
    };
    
    return count;
};

bool IsDirectory(char *name)
{
#ifdef _WIN32
    if (name[strlen(name) - 1] == '\\')
    {
        return true;
    }
#endif
    
#ifdef __linux__
    if (name[strlen(name) - 1] == '/')
    {
        return true;
    }
#endif
    return false;
};

bool IsSymLink(char *name)
{
    if(strlen(name) == 1 && name[0] == '.')
    {
        return true;
    }
    else if(strlen(name) == 2 && name[0] == '.' && name[1] == '.')
    {
        return true;
    }
    
    return false;
};

bool IsValidPathFilter(char *path, char *name)
{
    tinydir_dir dir;
    tinydir_open(&dir, path);
    char *current;
    
    while (dir.has_next)
    {
        tinydir_file file;
        tinydir_readfile(&dir, &file);
        
        current = file.name;
        
        if (file.is_dir)
        {
            int len = strlen(current);
#ifdef _WIN32
            current[len] = '\\';
#endif
#ifdef __linux__
            current[len] = '/';
#endif
            current[len + 1] = '\0';
        }
        
        if(strcmp(current, name) == 0)
        {
            return true;
        }
        
        tinydir_next(&dir);
    }
    
    tinydir_close(&dir);
    
    return false;
};

bool IsTopLevelDirectory(xstring *path)
{
#ifdef _WIN32
    if(XstringGetLength(path) == 3)
    {
        if(XstringGet(path)[1] == ':' && XstringGet(path)[2] == '\\')
        {
            return true;
        }
    }
#endif
    
#ifdef __linux__
    if(XstringGetLength(path) == 1)
    {
        if(XstringGet(path)[0] == '/')
        {
            return true;
        }
    }
#endif
    return false;
};

xstring *XstringCreate(char text[])
{
    xstring *str = (xstring*)malloc(sizeof(xstring));
    int len = strlen(text);
    str->data = (char*)calloc(len + 1, 1);
    memcpy(str->data, text, len);
    str->length = len;
    return str;
};

void XstringDestroy(xstring *str)
{
    free(str->data);
    free(str);
};

void XstringSet(xstring *str, char *text)
{
    int len = strlen(text);
    
    if(len == str->length)
    {
        memcpy(str->data, text, str->length);
    }
    else
    {
        str->data = (char*)realloc(str->data, len + 1);
        str->length = len;
        memcpy(str->data, text, len);
        str->data[str->length] = 0;
    }
};

char *XstringGet(xstring *str)
{
    return str->data;
};

int XstringGetLength(xstring *str)
{
    return str->length;
};

void XstringTruncateTail(xstring *str, int count)
{
    if(count == 0 || count > str->length - 1)
    {
        return;
    }
    int new_size = str->length - count;
    str->data = (char*)realloc(str->data, new_size + 1);
    str->length = new_size;
    str->data[str->length] = 0;
};

void XstringTruncateHead(xstring *str, int count)
{
    if(count == 0 || count > str->length - 1)
    {
        return;
    }
    int new_size = str->length - count;
    char *temp = (char*)malloc(new_size);
    
    for (int i = 0; i < new_size; ++i)
    {
        temp[i] = str->data[i + count];
    }
    
    str->data = (char*)realloc(str->data, new_size + 1);
    memcpy(str->data, temp, new_size);
    str->length = new_size;
    str->data[str->length] = 0;
    free(temp);
};

void XstringConcat(xstring *str, int arg_count, ...)
{
    va_list valist;
    int old_len = str->length;
    int total_length = old_len;
    int index = 0;
    int *sizes = (int*)malloc(arg_count * sizeof(int));
    
    va_start(valist, arg_count);
    for (int i = 0; i < arg_count; ++i)
    {
        sizes[i] = strlen(va_arg(valist, char*));
        total_length += sizes[i];
    }
    va_end(valist);
    
    str->length = total_length;
    str->data = (char*)realloc(str->data, total_length + 1);
    
    va_start(valist, arg_count);
    
    int offset = old_len;
    for (int i = 0; i < arg_count; ++i)
    {
        memmove(str->data + offset, va_arg(valist, char*), sizes[i]);
        offset += sizes[i];
    }
    va_end(valist);
    
    free(sizes);
    
    str->data[str->length] = 0;
};

int XstringIndexOfLastOccurrance(xstring *str, char ch)
{
    int len = str->length;
    
    for (int i = len - 1; i >= 0; --i)
    {
        if(str->data[i] == ch)
        {
            return i;
        }
    }
    
    return -1;
};

int XstringIndexOfFirstOccurrance(xstring *str, char ch)
{
    int len = str->length;
    
    for (int i = 0; i < len; ++i)
    {
        if(str->data[i] == ch)
        {
            return i;
        }
    }
    
    return - 1;
};

void XstringTrimLeadingWhitespace(xstring *str)
{
    char ch = ' ';
    int len = str->length;
    
    if(len < 2)
    {
        return;
    }
    
    while(true)
    {
        if(str->data[0] == ch)
        {
            XstringTruncateHead(str, 1);
        }
        else
        {
            return;
        }
    }
};

void XstringTrimTrailingWhitespace(xstring *str)
{
    char ch = ' ';
    int len = str->length;
    
    if(len < 2)
    {
        return;
    }
    
    while(true)
    {
        if(str->data[len - 1] == ch)
        {
            XstringTruncateTail(str, 1);
        }
        else
        {
            return;
        }
    }
};

void XstringTrimLeadingAndTrailingWhitespace(xstring *str)
{
    XstringTrimLeadingWhitespace(str);
    XstringTrimTrailingWhitespace(str);
};

void XstringTrimAllWhitespace(xstring *str)
{
    char ch = ' ';
    int len = str->length;
    int index = 0;
    
    char *temp = (char*)calloc(str->length + 1, 1);
    
    
    for (int i = 0; i < len; ++i)
    {
        if(str->data[i] != ' ')
        {
            memcpy(temp + index, str->data + i, 1);
            index++;
        }
    }
    
    str->length = index;
    str->data = (char*)realloc(str->data, str->length + 1);
    memcpy(str->data, temp, str->length);
    str->data[str->length] = 0;
};

bool XstringContainsSubstring(xstring *str, char *substr)
{
    int substr_len = strlen(substr);
    int str_len = str->length;
    int start = XstringIndexOfFirstOccurrance(str, substr[0]);
    
    if(substr_len > str_len || start == -1 || start > str_len - substr_len)
    {
        return false;
    }
    
    for (int i = 0; i < substr_len; ++i)
    {
        if(str->data[start + i] != substr[i])
        {
            return false;
        }
    }
    
    return true;
};

bool CommandKeyBinding(key_binding kb)
{
    if(app.e.key.keysym.sym == kb.key)
    {
        if(kb.mod2 == KMOD_NONE)
        {
            if(SDL_GetModState() & kb.mod1)
            {
                return true;
            }
        }
        else
        {
            if(SDL_GetModState() & kb.mod1 && SDL_GetModState() & kb.mod2)
            {
                return true;
            }
        }
    }
    
    return false;
};