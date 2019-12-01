#pragma once
#include "globals.h"

node *InputText(buffer *buf, node *cur_node);
node *InputReturn(buffer *buf, node *cur_node);
node *InputBackspace(buffer *buf, node *cur_node);
node *InputDelete(buffer *buf, node *cur_node);
node *InputLeft(buffer *buf, node *cur_node);
node *InputRight(buffer *buf, node *cur_node);
node *InputUp(buffer *buf, node *cur_node);
node *InputDown(buffer *buf, node *cur_node);
node *InputDown(buffer *buf, node *cur_node);
node *InputPageUp(buffer *buf, node *cur_node);
node *InputPageDown(buffer *buf, node *cur_node);
void InputHome(buffer *buf, node *cur_node);
void InputEnd(buffer *buf, node *cur_node);
void InputTab(buffer *buf, node *cur_node);