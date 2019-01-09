#ifndef __WGS_TYPES_H__
#define __WGS_TYPES_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

struct wgs_tokenizer_context;

typedef bool		wgs_bool;
typedef uint8_t		wgs_uint8;
typedef int8_t		wgs_int8;
typedef uint16_t	wgs_uint16;
typedef int16_t		wgs_int16;
typedef uint32_t	wgs_uint32;
typedef int32_t		wgs_int32;
typedef uint64_t	wgs_uint64;
typedef int64_t		wgs_int64;
typedef size_t		wgs_size_t;
typedef ptrdiff_t	wgs_ptrdiff;

typedef float		wgs_float;
typedef double		wgs_double;

struct wgs_vec2f{ wgs_float x, y; };
struct wgs_vec2i32{ wgs_int32 x, y; };

struct wgs_vec4f { wgs_float x, y, z, w; };
struct wgs_vec4i32 { wgs_int32 x, y, z, w; };

typedef char		wgs_char;
#define wgs_true	true
#define wgs_false	false
#define wgs_assert	assert
#define wgs_null	(0)

#define wgs_max(a, b) ((a)<(b)?(b):(a))
#define wgs_min(a, b) ((a)<(b)?(a):(b))

struct wgs_core;

#endif