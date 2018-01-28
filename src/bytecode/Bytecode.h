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
#ifndef BYTECODE_BYTECODE_H
#define BYTECODE_BYTECODE_H

#include <utils.h>

namespace bytecode {

namespace detail {

constexpr inline u8 build_opcode(u8 op, u8 = 0) {
	return op;
}

}

using Register = i8;

enum class Bytecode : u8 {
	Nop			= detail::build_opcode(0x00),

	Add			= detail::build_opcode(0x10, 3),
	Sub			= detail::build_opcode(0x11, 3),
	Mul			= detail::build_opcode(0x12, 3),
	Div			= detail::build_opcode(0x13, 2),

	Cpy			= detail::build_opcode(0x14, 2),
	Set			= detail::build_opcode(0x15, 5),

	FuncHead	= detail::build_opcode(0x20, 1),
	Ret			= detail::build_opcode(0x21, 1),
	PushArg		= detail::build_opcode(0x22, 1),
	Call		= detail::build_opcode(0x23, 5),


	Jlt			= detail::build_opcode(0x31, 6),

};

static_assert(sizeof(Bytecode) == sizeof(u8));



union Value {
	i64 integer;
	double real;
	void* object;
};

static_assert(sizeof(Value) == sizeof(u64));


}

#endif // BYTECODE_BYTECODE_H
