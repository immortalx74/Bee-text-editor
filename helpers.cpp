#include "helpers.h"

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
    if (name[strlen(name) - 1] == '\\')
    {
        return true;
    }
    
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

void XstringSet(xstring *str, char text[])
{
    int len = strlen(text);
    
    if(len == str->length)
    {
        memcpy(str->data, text, str->length);
    }
    else if(len > str->length)
    {
        memcpy(str->data, text, str->length);
        str->data[str->length] = 0;
    }
    else
    {
        memcpy(str->data, text, len);
        
        for (int i = len; i < str->length; ++i)
        {
            str->data[i] = 0;
        }
        
        str->length = len;
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
    int new_size = str->length - count;
    str->data = (char*)realloc(str->data, new_size + 1);
    str->length = new_size;
    str->data[str->length] = 0;
};

void XstringTruncateHead(xstring *str, int count)
{
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
};

void XstringConcat(xstring *str, int arg_count, ...)
{
    va_list valist;
    int total_length = 0;
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
    
    int offset = 0;
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