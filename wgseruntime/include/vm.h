#pragma once

#include "types.h"
#include "opcodes.h"

struct WGSEVM
{
	WGSESTACKELEMENT stack[65535];

	WGSESTACKELEMENT sp;
	WGSESTACKELEMENT ip;
};


#define WGSE_POP(vm) (vm->stack[--vm->sp])
#define WGSE_PUSH(vm, v) vm->stack[vm->sp++] = v

struct WGSEVM*				wgse_create_vm();
void						wgse_destroy_vm(struct WGSEVM* vm);


struct WGSETYPEINSTANCE*	wgse_execute(struct WGSEVM* vm, size_t* program, WGSEFUNCTION* function_table);
