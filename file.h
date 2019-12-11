#pragma once
#include <fstream>
#include <string> 
#include "globals.h"
#include "line.h"
#include "input.h"

void FileReadToBuffer(buffer *buf, char *filename);
void FileWriteToDisk(buffer *buf, char *filename);