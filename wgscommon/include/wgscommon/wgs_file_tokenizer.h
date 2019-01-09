#ifndef __WGS_FILE_TOKENIZER_H__
#define __WGS_FILE_TOKENIZER_H__


#include "wgscommon/types.h"

#define MAX_TOKEN_LEN (255)

enum wgs_token
{
	WGS_TOKEN_NONE,
	WGS_TOKEN_SPACE, // < >
	WGS_TOKEN_EOL, // \n
	WGS_TOKEN_IDENTIFIER, // abcde.df0_gg
	WGS_TOKEN_TEXT, // "sdfsdf"
	WGS_TOKEN_CHAR, // any printable character
	WGS_TOKEN_INTEGER, // +-1234
	WGS_TOKEN_FLOAT, // +-1234.344
	WGS_TOKEN_NUMBER, // 0..9
	WGS_TOKEN_ARROW_IN, // <-
	WGS_TOKEN_ARROW_OUT, // ->
	WGS_TOKEN_OPAREN, // (
	WGS_TOKEN_CPAREN, // )
	WGS_TOKEN_OBRACE, // {
	WGS_TOKEN_CBRACE, // }
	WGS_TOKEN_OSQBRACKET, // [
	WGS_TOKEN_CSQBRACKET, // ]
	WGS_TOKEN_OANGLEBRACKET, // <
	WGS_TOKEN_CANGLEBRACKET, // >
	WGS_TOKEN_SINGLEQUOTE, // '
	WGS_TOKEN_DOUBLEQUOTE, // "
	WGS_TOKEN_COLON, // :
	WGS_TOKEN_COMMA, // ,
	WGS_TOKEN_DOT, // .
	WGS_TOKEN_MINUS, // -
	WGS_TOKEN_PLUS, // +
	WGS_TOKEN_UNDERSCORE, // _
};

enum wgs_tokenizer_result
{
	WGS_TOKENIZER_RESULT_OK,
	WGS_TOKENIZER_RESULT_INVALID_ARG,
	WGS_TOKENIZER_RESULT_ERROR,
};

typedef void (*wgs_tokenizer_on_token)(struct wgs_tokenizer_context* context, const wgs_char* token, size_t lenght, enum wgs_token type);
typedef void (*wgs_tokenizer_on_error)(struct wgs_tokenizer_context* context, const wgs_char* error);

struct wgs_tokenizer_context
{
	struct wgs_core*		wgs;
	wgs_tokenizer_on_token	on_token;
	wgs_tokenizer_on_error	on_error;

	wgs_size_t				line;
	wgs_uint8				depth;
	void*					user_data;
};

void							wgs_tokenizer_init_context(struct wgs_tokenizer_context* context);
enum wgs_tokenizer_result		wgs_tokenizer_process(struct wgs_tokenizer_context* context, const void* buffer, wgs_size_t buffer_size);


#endif