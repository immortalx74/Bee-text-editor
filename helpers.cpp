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