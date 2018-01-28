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

#include <cstdio>

namespace bytecode {

using bc_ptr = const Bytecode*;


template<typename T>
static T next(bc_ptr& bc) {
	T v = *reinterpret_cast<const T*>(bc);
	bc += sizeof(T);
	return v;
}


static Value eval(bc_ptr bc, Value* stack) {
	Value* stack_top = stack;

	usize args = 0;

	u8 dst;
	u8 op_a;
	u8 op_b;
	i32 offset;

	auto read_dst =		[&]() { dst = next<Register>(bc); };
	auto read_op_a =	[&]() { op_a = next<Register>(bc); };
	auto read_op_b =	[&]() { op_b = next<Register>(bc); };
	auto read_ops =		[&]() { read_dst(); read_op_a(); read_op_b(); };
	auto read_offset =	[&]() { offset = next<i32>(bc); };

	for(;;) {
		Bytecode i = next<Bytecode>(bc);

		switch(i) {
			case Bytecode::Nop:
			break;

			case Bytecode::Add:
				read_ops();

				stack[dst].integer = stack[op_a].integer + stack[op_b].integer;
			break;

			case Bytecode::Sub:
				read_ops();

				stack[dst].integer = stack[op_a].integer - stack[op_b].integer;
			break;

			case Bytecode::Cpy:
				read_dst();
				read_op_a();

				stack[dst] = stack[op_a];
			break;

			case Bytecode::Set:
				read_dst();
				read_offset();

				stack[dst].integer = offset;
			break;


			case Bytecode::FuncHead:
				read_op_a();

				args = 0;
				stack_top = stack + op_a;
			break;

			case Bytecode::PushArg:
				read_op_a();

				stack_top[args++] = stack[op_a];
			break;

			case Bytecode::Ret:
				read_op_a();

				return stack[op_a];
			break;

			case Bytecode::Call:
				read_dst();
				read_offset();

				if(offset && bc[offset] != Bytecode::FuncHead) {
					fatal("Invalid call.");
				}
				args = 0;
				stack[dst] = eval(bc + offset, stack_top);
			break;

			case Bytecode::Jlt:
				read_op_a();
				read_op_b();
				read_offset();

				if(stack[op_a].integer < stack[op_b].integer) {
					bc = bc + offset;
				}
			break;

			default:
				printf("Invalid instruction: %x\n", int(i));
				fatal("Invalid instruction.");
		}
	}
}


Value eval(const Bytecode* bc) {
	static constexpr usize stack_size = 16 * 1024;
	Value* stack = new Value[stack_size];

	auto r = eval(bc, stack);

	delete[] stack;

	return r;

}

}
