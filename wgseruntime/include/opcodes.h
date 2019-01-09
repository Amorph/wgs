#pragma once

enum WGSEOpcode
{
	OP_END = 0,
	OP_STACK_PUSH,
	OP_STACK_POP,
	OP_CALL,

	OP_SIZE = 0x7FFF
};