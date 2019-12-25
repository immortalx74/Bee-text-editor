#pragma once
#include "globals.h"
#include "line.h"
#include "draw.h"
#include "undoredo.h"
#include <string>

void InsertCharacterAt(buffer *buf, int col);
void DeleteCharacterAt(buffer *buf, int col);

/*! Returns the number of UTF-8 characters in the given string. */
int U8_strlen(const char* string);

/*! Returns a pointer to the next UTF-8 character. */
const char* U8_next(const char* string);

/*! Returns the number of bytes in the UTF-8 multibyte character pointed at by 'character'. */
int U8_charsize(const char* character); 

/*! Inserts a UTF-8 string into 'string' at the given position.  Use a position of -1 to append.  Returns 0 when unable to insert the string. */
int U8_strinsert(char* string, int position, const char* source, int max_bytes);

/*! Erases the UTF-8 character at the given position, moving the subsequent characters down. */
void U8_strdel(char* string, int position);