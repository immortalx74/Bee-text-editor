#pragma once
#include <fstream>
#include <string> 
#include "globals.h"
#include "line.h"
#include "input.h"

void FileReadToBuffer(buffer *buf, char *filename);
void FileWriteToDisk(buffer *buf, char *filename);
void FileParseSettings();
void SetSetting(int index, xstring *value);
SDL_Color ExtractColorFromString(xstring *str);
key_binding ExtractKeyBindingFromString(xstring *str);
int GetKeymodFromString(xstring * str);