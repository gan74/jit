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
#ifndef BYTECODE_INSTRUCTION_H
#define BYTECODE_INSTRUCTION_H

#include "Bytecode.h"

namespace bytecode {

union Instruction {
	struct {
		Op op;
		u8 flags;
	} head;

	struct {
		u16 _;
		u16 dst;
		u16 a;
		u16 b;
	} bin_op;

	struct {
		u16 _;
		u16 regs;
	} func_head;

	struct {
		u16 _;
		u16 dst;
		i32 value;
	} value;


	constexpr Instruction(Op op) : head{op, 0} {
	}

	static Instruction bin(Op op, u16 dst, u16 a, u16 b) {
		return Instruction(op, dst, a, b);
	}

	static Instruction function(u16 reg_count) {
		return Instruction(Op::FuncHead, reg_count, 0);
	}

	static Instruction push_arg(u16 src) {
		return Instruction(Op::PushArg, src, 0);
	}

	static Instruction call(u16 dst, i32 offset) {
		return Instruction(Op::Call, dst, offset);
	}

	static Instruction ret(u16 src) {
		return Instruction(Op::Ret, src, 0);
	}

	static Instruction set(u16 dst, i32 value) {
		return Instruction(Op::Seti, dst, value);
	}

	static Instruction cmp(u16 a, u16 b) {
		return Instruction(Op::Cmp, 0, a, b);
	}

	static Instruction je(i32 offset) {
		return Instruction(Op::Je, 0, offset);
	}

	static Instruction jne(i32 offset) {
		return Instruction(Op::Jne, 0, offset);
	}

	private:
		constexpr static u16 head_16(Op op, u8 flags) {
			decltype(head) h{op, flags};
			return reinterpret_cast<u16&>(h);
		}

		constexpr Instruction(Op op, u16 dst, u16 a, u16 b) : bin_op{head_16(op, 0), dst, a, b} {
		}

		constexpr Instruction(Op op, u16 dst, i32 value) : value{head_16(op, 0), dst, value} {
		}


};

static_assert(offsetof(decltype(Instruction::bin_op), _) == 0);
static_assert(sizeof(Instruction) == sizeof(u64));

}

#endif // BYTECODE_INSTRUCTION_H
