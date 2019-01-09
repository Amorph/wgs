#include "vm.h"

#include "stdlib.h"

struct WGSEVM* wgse_create_vm()
{
	struct WGSEVM* vm = malloc(sizeof(struct WGSEVM));
	vm->sp = 0;
	vm->ip = 0;

	return vm;
}

void wgse_destroy_vm(struct WGSEVM* vm)
{
	free(vm);
}

struct WGSETYPEINSTANCE* wgse_execute(struct WGSEVM* vm, size_t* program, WGSEFUNCTION* function_table)
{
	vm->ip = 0;
	while (program[vm->ip] != OP_END)
	{
		switch (program[vm->ip])
		{
		case OP_STACK_PUSH:
			vm->stack[vm->sp++] = program[vm->ip + 1];
			vm->ip += 2;
			continue;
		case OP_STACK_POP:
			vm->sp -= program[vm->ip + 1];
			vm->ip += 2;
			continue;
		case OP_CALL:
			WGSEFUNCTION func = function_table[program[vm->ip + 1]];
			func(vm);
			vm->ip += 2;
		default:
			break;
		}
	}
	return 0;
}
