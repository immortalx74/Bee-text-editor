void InsertCharacterAt(buffer *buf, node *row, int col)
{
    U8_strinsert(row->data, buf->cursor.column, app.e.text.text, 128);
    //memset(row->data+buf->cursor.column, app.e.text.text[0], 1);
};