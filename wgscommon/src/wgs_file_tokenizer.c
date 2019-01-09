#include "wgscommon/wgs_file_tokenizer.h"

enum TokenCommand
{
	COMMAND_BREAK,
	COMMAND_JOIN,
	COMMAND_JOIN_AND_BREAK
};

static wgs_bool is_white_space(wgs_char ch)
{
	return ch == '\t' || ch == ' ' || ch == '\n' || ch == '\r' || ch == '\f' || ch == '\v' ? wgs_true : wgs_false;
}

static const wgs_char* describe(enum wgs_token token)
{
	switch (token)
	{
	case WGS_TOKEN_NONE: return "TOKEN_NONE";
	case WGS_TOKEN_SPACE: return "TOKEN_SPACE";
	case WGS_TOKEN_EOL: return "TOKEN_EOL";
	case WGS_TOKEN_IDENTIFIER: return "TOKEN_IDENTIFIER";
	case WGS_TOKEN_TEXT: return "TOKEN_TEXT";
	case WGS_TOKEN_CHAR: return "TOKEN_CHAR";
	case WGS_TOKEN_INTEGER: return "TOKEN_INTEGER";
	case WGS_TOKEN_FLOAT: return "TOKEN_FLOAT";
	case WGS_TOKEN_NUMBER: return "TOKEN_NUMBER";
	case WGS_TOKEN_ARROW_IN: return "TOKEN_IN";
	case WGS_TOKEN_ARROW_OUT: return "TOKEN_OUT";
	case WGS_TOKEN_OPAREN: return "TOKEN_OPAREN";
	case WGS_TOKEN_CPAREN: return "TOKEN_CPAREN";
	case WGS_TOKEN_OBRACE: return "TOKEN_OBRACE";
	case WGS_TOKEN_CBRACE: return "TOKEN_CBRACE";
	case WGS_TOKEN_OSQBRACKET: return "TOKEN_OSQBRACKET";
	case WGS_TOKEN_CSQBRACKET: return "TOKEN_CSQBRACKET";
	case WGS_TOKEN_OANGLEBRACKET: return "TOKEN_OANGLEBRACKET";
	case WGS_TOKEN_CANGLEBRACKET: return "TOKEN_CANGLEBRACKET";
	case WGS_TOKEN_SINGLEQUOTE: return "TOKEN_SINGLEQUOTE";
	case WGS_TOKEN_DOUBLEQUOTE: return "TOKEN_DOUBLEQUOTE";
	case WGS_TOKEN_COLON: return "TOKEN_COLON";
	case WGS_TOKEN_COMMA: return "TOKEN_COMMA";
	case WGS_TOKEN_DOT: return "TOKEN_DOT";
	case WGS_TOKEN_MINUS: return "TOKEN_MINUS";
	case WGS_TOKEN_PLUS: return "TOKEN_PLUS";
	case WGS_TOKEN_UNDERSCORE: return "TOKEN_UNDERSCORE";
	default: return "ERROR";
	}
}

static enum wgs_token classify(wgs_char ch)
{
	switch (ch)
	{
	case '\n': return WGS_TOKEN_EOL;
	case ' ': return WGS_TOKEN_SPACE;
	case '(': return WGS_TOKEN_OPAREN;
	case ')': return WGS_TOKEN_CPAREN;
	case '{': return WGS_TOKEN_OBRACE;
	case '}': return WGS_TOKEN_CBRACE;
	case '[': return WGS_TOKEN_OSQBRACKET;
	case ']': return WGS_TOKEN_CSQBRACKET;
	case '<': return WGS_TOKEN_OANGLEBRACKET;
	case '>': return WGS_TOKEN_CANGLEBRACKET;
	case '\'': return WGS_TOKEN_SINGLEQUOTE;
	case '"': return WGS_TOKEN_DOUBLEQUOTE;
	case ':': return WGS_TOKEN_COLON;
	case ',': return WGS_TOKEN_COMMA;
	case '.': return WGS_TOKEN_DOT;
	case '-': return WGS_TOKEN_MINUS;
	case '+': return WGS_TOKEN_PLUS;
	case '_': return WGS_TOKEN_UNDERSCORE;
	default:
		if (ch >= '0' && ch <= '9')
			return WGS_TOKEN_NUMBER;

		if (ch <= 0x1f || ch == 0x7f)
			return WGS_TOKEN_NONE;
	}
	return WGS_TOKEN_CHAR;
}



void wgs_tokenizer_init_context(struct wgs_tokenizer_context* context)
{
	context->on_token = 0;
	context->on_error = 0;
	context->line = 0;
	context->depth = 0; 
	context->user_data = 0;
}

enum wgs_tokenizer_result wgs_tokenizer_process(
	struct wgs_tokenizer_context* context, const void* buffer, wgs_size_t buffer_size)
{
	if (!context || !buffer)
		return WGS_TOKENIZER_RESULT_INVALID_ARG;

	const wgs_char* cursor = (wgs_char*)buffer;
	enum wgs_token cursor_token = WGS_TOKEN_NONE;
	const wgs_char* end = cursor + buffer_size;

	const wgs_char* token_start = wgs_null;
	enum wgs_token token_type = WGS_TOKEN_NONE;

	enum TokenCommand command = COMMAND_JOIN;

	while (cursor < end)
	{
		while (!token_start && is_white_space(*cursor))
		{
			if (*cursor == '\n')
			{
				context->on_token(context, cursor, 1, WGS_TOKEN_EOL);
				context->line++;
				context->depth = 0;
			}
			else if (*cursor == '\t')
				context->depth++;
			cursor++;
		}

		cursor_token = classify(*cursor);

		if (!token_start)
		{
			token_start = cursor;
			token_type = cursor_token;
			cursor++;
			continue;
		}
		command = COMMAND_JOIN;

		switch (token_type)
		{
		case WGS_TOKEN_CHAR:
			switch (cursor_token)
			{
				// Allowed join of CHAR+CHAR, CHAR+DOT, CHAR+DIGIT, CHAR+UNDERSCORE
			case WGS_TOKEN_CHAR:
			case WGS_TOKEN_DOT:
			case WGS_TOKEN_NUMBER:
			case WGS_TOKEN_UNDERSCORE:
				break;
			default:
				token_type = WGS_TOKEN_IDENTIFIER;
				command = COMMAND_BREAK;
			}
		case WGS_TOKEN_DOUBLEQUOTE:
			switch (cursor_token)
			{
				// Skip any char except " or endl
			case WGS_TOKEN_DOUBLEQUOTE:
				command = COMMAND_JOIN_AND_BREAK;
				token_type = WGS_TOKEN_TEXT;
				break;
			case WGS_TOKEN_EOL:
				//ERROR!
				context->on_error(context, "Not closed quoted string");
				command = COMMAND_BREAK;
			}
			break;
		case WGS_TOKEN_OANGLEBRACKET: // <
			if (cursor_token == WGS_TOKEN_MINUS) // <-
			{
				token_type = WGS_TOKEN_ARROW_IN;
				command = COMMAND_JOIN_AND_BREAK;
				break;
			}
		case WGS_TOKEN_MINUS: // -
			if (cursor_token == WGS_TOKEN_CANGLEBRACKET) // ->
			{
				token_type = WGS_TOKEN_ARROW_OUT;
				command = COMMAND_JOIN_AND_BREAK;
				break;
			}
		case WGS_TOKEN_PLUS: // +
			if (cursor_token == WGS_TOKEN_NUMBER) // +1
				token_type = WGS_TOKEN_NUMBER;
			else
				command = COMMAND_BREAK;
			break;
		case WGS_TOKEN_NUMBER:
			if (cursor_token != WGS_TOKEN_DOT && cursor_token != WGS_TOKEN_NUMBER)
			{
				token_type = WGS_TOKEN_INTEGER;
				command = COMMAND_BREAK;
			}
			if (cursor_token == WGS_TOKEN_DOT)
				token_type = WGS_TOKEN_FLOAT;
			break;
		case WGS_TOKEN_FLOAT:
			if (cursor_token != WGS_TOKEN_NUMBER)
				command = COMMAND_BREAK;
			break;
		default:
			command = COMMAND_BREAK;
		}

		switch (command)
		{
		case COMMAND_JOIN_AND_BREAK:
			cursor++;
		case COMMAND_BREAK:
			context->on_token(context, token_start, cursor - token_start, token_type);
			token_start = wgs_null;
			break;
		case COMMAND_JOIN:
			cursor++;
		}
	}

	return WGS_TOKENIZER_RESULT_OK;
}
