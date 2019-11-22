#pragma once
#include "globals.h"

node *InputReturn(buffer *buf, node *cur_node);
node *InputBackspace(buffer *buf, node *cur_node);
node *InputDelete(buffer *buf, node *cur_node);
node *InputLeft(buffer *buf, node *cur_node);
node *InputRight(buffer *buf, node *cur_node);
node *InputUp(buffer *buf, node *cur_node);
node *InputDown(buffer *buf, node *cur_node);
node *InputDown(buffer *buf, node *cur_node);
void InputTab(buffer *buf, node *cur_node);