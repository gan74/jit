/*******************************
Copyright (c) 2016-2017 Gr�goire Angerand

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
**********************************/

#include "eval.h"

namespace bytecode {

static Value eval(Instruction* bc, Value* stack) {
	Value* stack_top = stack;
	usize args = 0;
	bool is_equal = false;
	for(;; ++bc) {
		Instruction i = *bc;

		switch(i.head.op) {
			case Op::Nop:
			break;

			case Op::FuncHead:
				args = 0;
				stack_top = stack + i.func_head.regs;
			break;

			case Op::Mov:
				stack[i.bin_op.dst] = stack[i.bin_op.a];
			break;

			case Op::Seti:
				stack[i.value.dst].integer = i.value.value;
			break;

			case Op::PushArg:
				stack_top[args++] = stack[i.bin_op.dst];
			break;

			case Op::Call:
				if((bc + i.value.value)->head.op != Op::FuncHead) {
					fatal("Invalid call.");
				}
				args = 0;
				stack[i.value.dst] = eval(bc + i.value.value, stack_top);
			break;

			case Op::Addi:
				stack[i.bin_op.dst].integer = stack[i.bin_op.a].integer + stack[i.bin_op.b].integer;
			break;

			case Op::Subi:
				stack[i.bin_op.dst].integer = stack[i.bin_op.a].integer - stack[i.bin_op.b].integer;
			break;

			case Op::Muli:
				stack[i.bin_op.dst].integer = stack[i.bin_op.a].integer * stack[i.bin_op.b].integer;
			break;

			case Op::Cmp:
				is_equal = stack[i.bin_op.a].integer == stack[i.bin_op.b].integer;
			break;

			case Op::Je:
				if(is_equal) {
					bc = bc + i.value.value;
				}
			break;

			case Op::Jne:
				if(!is_equal) {
					bc = bc + i.value.value;
				}
			break;

			case Op::Jmp:
				bc = bc + i.value.value;
			break;

			case Op::Ret:
				return stack[i.bin_op.dst];
			break;

			default:
				fatal("Invalid instruction.");
		}
	}
}


Value eval(Instruction* bc) {
	static constexpr usize stack_size = 16 * 1024;
	Value* stack = new Value[stack_size];

	auto r = eval(bc, stack);

	delete[] stack;

	return r;
}

}
