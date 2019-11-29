#pragma once
#include <fstream>
#include <string> 
#include "globals.h"
#include "line.h"
#include "input.h"

node *FileReadToBuffer(buffer *buf, char *filename);
void FileWriteToDisk(buffer *buf, char *filename);