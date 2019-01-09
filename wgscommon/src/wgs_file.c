#include "wgscommon/wgs_file.h"

#include "wgscommon/wgs_file_tokenizer.h"
#include "wgscommon/wgs_core.h"

//TEMP!
#include <inttypes.h>
#include <stdio.h>

static wgs_size_t DATA_CHUNK_SIZE = 4096;

struct _wgs_parsing_context
{
	struct wgs_file* file;
	struct wgs_file_node* node_stack[255];
	wgs_uint8	node_stack_index;
	wgs_int8	current_depth;
	wgs_bool	new_line;
	wgs_bool	parentheses_child;
};

static void wgs_parsing_init_context(struct _wgs_parsing_context* context, struct wgs_file* file)
{
	context->file = file;
	context->node_stack[0] = file->root;
	context->node_stack_index = 0;
	context->current_depth = -1; // depth of root node
	context->new_line = wgs_true;
	context->parentheses_child = wgs_false;
}

static wgs_int16 update_tab_dist(struct wgs_tokenizer_context* tokenizer_context)
{
	struct wgs_core* wgs = tokenizer_context->wgs;
	struct _wgs_parsing_context* parsing_context = (struct _wgs_parsing_context*)tokenizer_context->user_data;

	wgs_int16 depth_dist = tokenizer_context->depth - parsing_context->current_depth;
	if (depth_dist > 1)
	{
		wgs->log.printf(wgs, "Error: %s, line:%zd\n", "Wrong block ident", tokenizer_context->line);
		depth_dist = 1;
	}
	else if (depth_dist < 0)
	{
		parsing_context->node_stack_index += depth_dist;
		depth_dist = 0;
		parsing_context->current_depth = tokenizer_context->depth;
	}
	return depth_dist;
}

static struct wgs_file_node* convert_token(struct wgs_tokenizer_context* tokenizer_context, const wgs_char* token, wgs_size_t lenght, enum wgs_token type)
{
	struct wgs_core* wgs = tokenizer_context->wgs;
	struct _wgs_parsing_context* parsing_context = (struct _wgs_parsing_context*)tokenizer_context->user_data;

	//struct wgs_file_node* new_node
	switch (type)
	{
	case WGS_TOKEN_NONE:
	case WGS_TOKEN_SPACE:
	case WGS_TOKEN_EOL:
		return wgs_null;
	case WGS_TOKEN_ARROW_IN:
	case WGS_TOKEN_ARROW_OUT:
	case WGS_TOKEN_IDENTIFIER:
		return wgsfile_create_node(parsing_context->file, WGS_FILE_NODE_IDENTIFIER, token, lenght);
	case WGS_TOKEN_TEXT:
		return wgsfile_create_node(parsing_context->file, WGS_FILE_NODE_STR, token, lenght);
	case WGS_TOKEN_CHAR:
		return wgs_null;
	case WGS_TOKEN_INTEGER:
	{
		wgs_file_integer i;
		int ret = _snscanf(token, lenght, "%" SCNd64, &i);
		if (ret == 0)
			return wgs_null;
		return wgsfile_create_node(parsing_context->file, WGS_FILE_NODE_INT, (const wgs_char*)&i, sizeof(i));
	}
	case WGS_TOKEN_FLOAT:
	{
		wgs_file_float i;
		int ret = _snscanf(token, lenght, "%f", &i);
		if (ret == 0)
			return wgs_null;
		return wgsfile_create_node(parsing_context->file, WGS_FILE_NODE_FLOAT, (const wgs_char*)&i, sizeof(i));
	}
	case WGS_TOKEN_NUMBER:
	case WGS_TOKEN_OPAREN:
	case WGS_TOKEN_CPAREN:
	case WGS_TOKEN_OBRACE:
	case WGS_TOKEN_CBRACE:
	case WGS_TOKEN_OSQBRACKET:
	case WGS_TOKEN_CSQBRACKET:
	case WGS_TOKEN_OANGLEBRACKET:
	case WGS_TOKEN_CANGLEBRACKET:
	case WGS_TOKEN_SINGLEQUOTE:
	case WGS_TOKEN_DOUBLEQUOTE:
	case WGS_TOKEN_COLON:
	case WGS_TOKEN_COMMA:
	case WGS_TOKEN_DOT:
	case WGS_TOKEN_MINUS:
	case WGS_TOKEN_PLUS:
	case WGS_TOKEN_UNDERSCORE:
		wgs_assert(wgs_false);
	default:
		break;
	}
	return wgs_null;
}

static void on_token(struct wgs_tokenizer_context* tokenizer_context, 
	const wgs_char* token, wgs_size_t lenght, enum wgs_token type)
{
	enum ADDING_MODE
	{
		ADD_NONE,
		ADD_CHILD,
		ADD_SIBLING,
		ADD_RIGHT
	};
	struct wgs_core* wgs = tokenizer_context->wgs;
	struct _wgs_parsing_context* parsing_context = (struct _wgs_parsing_context*)tokenizer_context->user_data;
	if (!parsing_context)
		return;

	if (type == WGS_TOKEN_EOL)
	{
		// add check pair and parentesis
		parsing_context->new_line = wgs_true;
		return;
	}
	enum ADDING_MODE adding_mode = ADD_RIGHT;
	if (parsing_context->new_line)
	{
		parsing_context->new_line = wgs_false;
		wgs_int16 depth_dist = update_tab_dist(tokenizer_context);
		adding_mode = depth_dist ? ADD_CHILD : ADD_SIBLING;
	}

	struct wgs_file_node* parent_node = parsing_context->node_stack[parsing_context->node_stack_index-1];
	struct wgs_file_node* current_node = parsing_context->node_stack[parsing_context->node_stack_index];

	struct wgs_file_node* new_node = wgs_null;

	if (type == WGS_TOKEN_COLON)
	{
		struct wgs_file_node* prev_prev_node = wgs_null;
		struct wgs_file_node* prev_node = current_node;

		while (prev_node->links.right != wgs_null)
		{
			prev_prev_node = prev_node;
			prev_node = prev_node->links.right;
		}
		assert(prev_node);
		if(prev_prev_node)
			prev_prev_node->links.right = wgs_null;
		new_node = wgsfile_create_node(parsing_context->file, WGS_FILE_NODE_PAIR, wgs_null, 0);
		wgsfile_add_child(parsing_context->file, new_node, prev_node);
		if(prev_prev_node)
			current_node = prev_prev_node;

		parsing_context->node_stack_index++;
		parsing_context->node_stack[parsing_context->node_stack_index] = new_node;
	}
	else if(type == WGS_TOKEN_OPAREN)
	{
		new_node = wgsfile_create_node(parsing_context->file, WGS_FILE_NODE_TUPLE, wgs_null, 0);
		parsing_context->node_stack_index++;
		parsing_context->node_stack[parsing_context->node_stack_index] = new_node;
		parsing_context->parentheses_child = wgs_true;
	}
	else if (type == WGS_TOKEN_CPAREN)
	{
		parsing_context->parentheses_child = wgs_true;
		parsing_context->node_stack[parsing_context->node_stack_index--] = wgs_null;
		parsing_context->node_stack[parsing_context->node_stack_index--] = wgs_null;
		return;
	}
	else if (type == WGS_TOKEN_COMMA)
	{
		parsing_context->parentheses_child = wgs_true;
		parsing_context->node_stack[parsing_context->node_stack_index--] = wgs_null;
		return;
	}

	if(!new_node)
		new_node = convert_token(tokenizer_context, token, lenght, type);

	if (new_node == wgs_null)
		return;

	if (current_node->type == WGS_FILE_NODE_PAIR)
	{
		adding_mode = ADD_NONE;
		wgsfile_add_child(parsing_context->file, current_node, new_node);
		parsing_context->node_stack[parsing_context->node_stack_index] = wgs_null;
		parsing_context->node_stack_index--;
	}
	else if (current_node->type == WGS_FILE_NODE_TUPLE)
	{
		if (parsing_context->parentheses_child)
		{
			parsing_context->parentheses_child = wgs_false;
			adding_mode = ADD_CHILD;
		}
	}

	switch (adding_mode)
	{
	case ADD_RIGHT:
		wgsfile_add_right(parsing_context->file, current_node, new_node);
		break;
	case ADD_CHILD:
		parent_node = current_node;
		parsing_context->node_stack_index++;
		parsing_context->current_depth = tokenizer_context->depth;
	case ADD_SIBLING:
		wgsfile_add_child(parsing_context->file, parent_node, new_node);
		parsing_context->node_stack[parsing_context->node_stack_index] = new_node;
		break;
	
	default:
		break;
	}
}

static void on_error(struct wgs_tokenizer_context* context, const wgs_char* error)
{
	context->wgs->log.printf(context->wgs, "Error: %s, line:%zd\n", error, context->line);
}

struct wgs_file* wgsfile_open(struct wgs_core* wgs, const void* data, wgs_size_t size)
{
	struct wgs_file* file = wgs->mem.malloc(wgs, sizeof(struct wgs_file));
	file->wgs = wgs;
	file->data = wgs_null;

	file->root = wgsfile_create_node(file, WGS_FILE_NODE_ROOT, "Root", 5);

	{
		struct _wgs_parsing_context parsing_context;
		struct wgs_tokenizer_context tokenizer_context;

		wgs_parsing_init_context(&parsing_context, file);
		wgs_tokenizer_init_context(&tokenizer_context);

		tokenizer_context.wgs = wgs;
		tokenizer_context.user_data = &parsing_context;
		tokenizer_context.on_token = on_token;
		tokenizer_context.on_error = on_error;

		wgs_tokenizer_process(&tokenizer_context, data, size);
	}

	wgs_char* cursor = (wgs_char*)data;
	
	return file;
}


struct wgs_file_node* wgsfile_create_node(struct wgs_file* file, enum wgs_file_node_type type, const wgs_char* tag, wgs_size_t tag_len)
{
	struct wgs_file_node* output = wgsfile_store_data(file, wgs_null, sizeof(struct wgs_file_node));
	output->type = type;
	output->tag = wgs_null;
	output->links.next = wgs_null;
	output->links.child = wgs_null;
	output->links.right = wgs_null;

	if (tag_len)
	{
		output->tag = (wgs_char*)wgsfile_store_data(file, tag, tag_len + 1);
		output->tag[tag_len] = 0;
	}

	return output;
}

struct wgs_file_node* wgsfile_add_child(struct wgs_file* file, struct wgs_file_node* node, struct wgs_file_node* placing_node)
{
	if (node->links.child)
	{
		struct wgs_file_node* last_sibling = node->links.child;
		while (last_sibling->links.next)
			last_sibling = last_sibling->links.next;
		last_sibling->links.next = placing_node;
	}
	else
		node->links.child = placing_node;

	return placing_node;
}

struct wgs_file_node* wgsfile_add_right(struct wgs_file* file, struct wgs_file_node* node, struct wgs_file_node* placing_node)
{
	if (node->links.right)
	{
		struct wgs_file_node* last_sibling = node->links.right;
		while (last_sibling->links.right)
			last_sibling = last_sibling->links.right;
		last_sibling->links.right = placing_node;
	}
	else
		node->links.right = placing_node;

	return placing_node;
}

void* wgsfile_store_data(struct wgs_file* file, const void* data, wgs_size_t size)
{
	struct wgs_file_data_chunk* data_chunk = file->data;
	struct wgs_file_data_chunk* last_data_chunk = wgs_null;

	while (data_chunk)
	{
		last_data_chunk = data_chunk;

		if (data_chunk->size - data_chunk->used > size)
			break;

		data_chunk = data_chunk->next;
	}
	if (!data_chunk)
	{
		data_chunk = file->wgs->mem.malloc(file->wgs, DATA_CHUNK_SIZE);
		data_chunk->size = 4096;
		data_chunk->used = sizeof(struct wgs_file_data_chunk);
		data_chunk->next = wgs_null;
		wgs_assert(data_chunk->size > size);
		if (last_data_chunk)
			last_data_chunk->next = data_chunk;
		else
			file->data = data_chunk;
	}

	wgs_assert(data_chunk);
	wgs_assert(data_chunk->size - data_chunk->used > size);

	void* target_place = ((wgs_uint8*)data_chunk) + data_chunk->used;
	if(data)
		file->wgs->mem.memcpy(file->wgs, target_place, data, size);
	data_chunk->used += size;

	return target_place;
}

void wgsfile_printer_init(struct wgs_file* file, struct wgs_file_print_context* printer)
{
	printer->file = file;
	printer->node = file->root;
	printer->prev_node = wgs_null;
	printer->node_stack[0] = file->root;
	printer->node_stack_index = 0;
	printer->depth = -1;
}

enum NodeDirection
{
	ND_NONE,
	ND_RIGHT,
	ND_CHILD,
	ND_NEXT,
};

struct wgs_file_node* node_get_parent(struct wgs_file_print_context* printer)
{
	wgs_int16 stack_idx = printer->node_stack_index;
	while (stack_idx > 0)
	{
		struct wgs_file_node* current = printer->node_stack[stack_idx];
		struct wgs_file_node* prev = printer->node_stack[stack_idx-1];

		if (prev->links.child == current)
			return prev;

		stack_idx--;
	}
	return wgs_null;
}

static void node_on_enter(struct wgs_file_print_context* printer, enum NodeDirection dir)
{
	struct wgs_file_node* parent = node_get_parent(printer);
	wgs_bool isTupleContainer = parent ? parent->type == WGS_FILE_NODE_TUPLE : wgs_false;
	if (!isTupleContainer)
	{
		if (dir == ND_CHILD)
			printer->depth++;

		if (dir == ND_CHILD || dir == ND_NEXT)
		{
			printf("\n");
			for (wgs_int16 i = 0; i < printer->depth; i++)
				printf("\t");
		}
		else if (dir == ND_RIGHT)
			printf(" ");
	}
	else
	{
		if (dir == ND_NEXT)
		{
			printf(", ");
		}
	}
	
	switch (printer->node->type)
	{
	case WGS_FILE_NODE_ROOT:
		printf("Root\n");
		break;
	case WGS_FILE_NODE_IDENTIFIER:
		printf("%s", printer->node->tag);
		break;
	case WGS_FILE_NODE_STR:
		printf("%s", printer->node->tag);
		break;
	case WGS_FILE_NODE_TUPLE:
		printf("(");
		break;
	case WGS_FILE_NODE_FLOAT:
		printf("%g", *((float*)printer->node->tag));
		break;
	case WGS_FILE_NODE_INT:
		printf("%lld", *((wgs_file_integer*)printer->node->tag));
		break;
	}
}

static void node_on_exit(struct wgs_file_print_context* printer, enum NodeDirection dir)
{
	struct wgs_file_node* parent = node_get_parent(printer);
	wgs_bool isTupleContainer = parent ? parent->type == WGS_FILE_NODE_TUPLE : wgs_false;
	if (!isTupleContainer)
	{
		if (parent && parent->links.child == printer->node)
			printer->depth--;
	}
	switch (printer->node->type)
	{
	case WGS_FILE_NODE_TUPLE:
		printf(")");
		break;
	}
}

static void push_node(struct wgs_file_print_context* printer, struct wgs_file_node* node, enum NodeDirection dir)
{
	printer->node = printer->node_stack[++printer->node_stack_index] = node;
	node_on_enter(printer, dir);
}

static void pop_node(struct wgs_file_print_context* printer, enum NodeDirection dir)
{
	node_on_exit(printer, dir);

	printer->prev_node = printer->node_stack[printer->node_stack_index];
	printer->node_stack[printer->node_stack_index--] = wgs_null;
}

wgs_size_t wgsfile_printer_fill(struct wgs_file_print_context* printer, wgs_char* buff, wgs_size_t size)
{

	while (true)
	{
		if (printer->node_stack_index < 0)
			break;

		printer->node = printer->node_stack[printer->node_stack_index];

		enum NodeDirection direction = ND_NONE;
		if (printer->prev_node)
		{
			if (printer->node->links.right == printer->prev_node)
				direction = ND_RIGHT;
			else if (printer->node->links.child == printer->prev_node)
				direction = ND_CHILD;
			else if (printer->node->links.next == printer->prev_node)
				direction = ND_NEXT;
		}
		
		switch (direction)
		{
		case ND_NONE:
			if (printer->node->links.right)
				push_node(printer, printer->node->links.right, ND_RIGHT);
			else if (printer->node->links.child)
				push_node(printer, printer->node->links.child, ND_CHILD);
			else if (printer->node->links.next)
			{
				push_node(printer, printer->node->links.next, ND_NEXT);
			}
			else
				pop_node(printer, ND_NONE);
			break;
		case ND_RIGHT:
			if (printer->node->links.child)
				push_node(printer, printer->node->links.child, ND_CHILD);
			else if (printer->node->links.next)
			{
				push_node(printer, printer->node->links.next, ND_NEXT);
			}
			else
				pop_node(printer, ND_RIGHT);
			break;
		case ND_CHILD:
			if (printer->node->links.next)
			{
				push_node(printer, printer->node->links.next, ND_NEXT);
			}
			else
				pop_node(printer, ND_CHILD);
			break;
		case ND_NEXT:
			pop_node(printer, ND_NEXT);
			break;
		}
	}
	return 0;
}
