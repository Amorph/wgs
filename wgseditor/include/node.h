#pragma once

#include "wgscommon/types.h"

struct wgse_editor;
class WGSEditor;

struct wgse_node
{
	unsigned int ID;

	ImVec2 position;
	ImVec2 size;

	int	input_count;
	int output_count;

	struct wgse_node* next;
	struct wgse_node* prev;
};

void draw_node(WGSEditor* editor, struct wgse_node* node);
