#pragma once
#include "globals.h"
#include "tinydir.h"
#include <string.h>
#include <iostream>
#include "stdarg.h"

int GetDigitCount(int n);
bool IsDirectory(char *name);
bool IsSymLink(char *name);
bool IsValidPathFilter(char *path, char *name);
bool IsTopLevelDirectory(xstring *path);

xstring *XstringCreate(char text[]);
void XstringDestroy(xstring *str);
void XstringSet(xstring *str, char *text);
char *XstringGet(xstring *str);
int XstringGetLength(xstring *str);
void XstringTruncateTail(xstring *str, int count);
void XstringTruncateHead(xstring *str, int count);
void XstringConcat(xstring *str, int arg_count, ...);
int XstringIndexOfLastOccurrance(xstring *str, char ch);
int XstringIndexOfFirstOccurrance(xstring *str, char ch);