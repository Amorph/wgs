#ifndef __WGS_CORE_H__
#define __WGS_CORE_H__

#include "wgscommon/types.h"

#define WGS_CORE_C_IMPLEMENTATION 1

struct wgs_mem
{
	// allocation
	void*(*malloc)(struct wgs_core*, wgs_size_t size);
	void(*free)(struct wgs_core*, void* data);

	// memory operations
	void(*memcpy)(struct wgs_core*, void* dest, const void* src, wgs_size_t count);
};

struct wgs_log
{
	wgs_int32(*printf)(struct wgs_core*, const wgs_char* format, ...);
};

struct wgs_core
{
	struct wgs_mem mem;
	struct wgs_log log;
};

#if WGS_CORE_C_IMPLEMENTATION
struct wgs_core* wgs_C_core();
#endif


#endif