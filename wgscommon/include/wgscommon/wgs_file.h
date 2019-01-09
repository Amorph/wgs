#ifndef __WGS_FILE_H__
#define __WGS_FILE_H__

#include "wgscommon/types.h"

enum wgs_file_node_type
{
	WGS_FILE_NODE_ROOT,
	WGS_FILE_NODE_IDENTIFIER,
	WGS_FILE_NODE_STR,
	WGS_FILE_NODE_INT,
	WGS_FILE_NODE_FLOAT,
	WGS_FILE_NODE_TUPLE,
	WGS_FILE_NODE_PAIR
};

typedef wgs_int64	wgs_file_integer;
typedef wgs_float	wgs_file_float;

struct wgs_file_node
{
	wgs_char*					tag;
	enum wgs_file_node_type		type;
	struct
	{
		struct wgs_file_node*	child;
		struct wgs_file_node*	next;
		struct wgs_file_node*	right;
	}links;
};

struct wgs_file_data_chunk
{
	wgs_size_t size;
	wgs_size_t used;
	struct wgs_file_data_chunk* next;
};

struct wgs_file
{
	struct wgs_core*			wgs;
	struct wgs_file_node*		root;
	struct wgs_file_data_chunk*	data;
};

struct wgs_file_print_context
{
	struct wgs_file*		file;
	struct wgs_file_node*	node;
	struct wgs_file_node*	prev_node;
	struct wgs_file_node*	node_stack[255];
	wgs_int16				node_stack_index;
	wgs_int16				depth;
};

struct wgs_file*		wgsfile_open(struct wgs_core* wgs, const void* data, wgs_size_t size);

struct wgs_file_node*	wgsfile_create_node(struct wgs_file* file, enum wgs_file_node_type type, const wgs_char* tag, wgs_size_t tag_len);

struct wgs_file_node*	wgsfile_add_child(struct wgs_file* file, struct wgs_file_node* node, struct wgs_file_node* placing_node);
struct wgs_file_node*	wgsfile_add_right(struct wgs_file* file, struct wgs_file_node* node, struct wgs_file_node* placing_node);

void*					wgsfile_store_data(struct wgs_file* file, const void* data, wgs_size_t size);

void					wgsfile_printer_init(struct wgs_file* file, struct wgs_file_print_context* printer);
wgs_size_t				wgsfile_printer_fill(struct wgs_file_print_context* printer, wgs_char* buff, wgs_size_t size);


#endif