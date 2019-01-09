#include "wgscommon/wgs_core.h"


#if WGS_CORE_C_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

static void* wgs_c_malloc(struct wgs_core* wgs, wgs_size_t size)
{
	return malloc(size);
}

static void wgs_c_free(struct wgs_core* wgs, void* data)
{
	free(data);
}

static void wgs_c_memcpy(struct wgs_core* wgs, void* dest, const void* src, wgs_size_t count)
{
	memcpy(dest, src, count);
}

wgs_int32 wgs_c_printf(struct wgs_core* wgs, const wgs_char* format, ...)
{
	wgs_int32 cnt;
	va_list args;

	va_start(args, format);
	cnt = vprintf(format, args);
	va_end(args);

	return cnt;
}

static struct wgs_core wgs_C_coreImpl = {
	.mem = {
		.malloc = wgs_c_malloc,
		.free = wgs_c_free,
		.memcpy = wgs_c_memcpy
	},
	.log = {
		.printf = wgs_c_printf
	}
};

struct wgs_core* wgs_C_core()
{
	return &wgs_C_coreImpl;
}

#endif
