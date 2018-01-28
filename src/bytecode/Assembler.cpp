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

#include "Assembler.h"

#include <cstdio>

namespace bytecode {

void Assembler::dump() const {
	for(auto i : _bytes) {
		printf("%x ", int(i));
	}
	printf("\n");
}

void Assembler::forward_label(u32 label) {
	u32 diff =  _bytes.size() - label;
	*reinterpret_cast<u32*>(&_bytes[label - sizeof(u32)]) = diff;
}

Assembler::Label Assembler::label() const {
	return Label(_bytes.size());
}


void Assembler::function_head(Register regs) {
	push(Bytecode::FuncHead, regs);
}

void Assembler::ret(Register src) {
	push(Bytecode::Ret, src);
}

void Assembler::push_arg(Register src) {
	push(Bytecode::PushArg, src);
}

void Assembler::call(Register dst, Label fn) {
	push(Bytecode::Call, dst);
	push_i32(fn - label() - sizeof(i32));
}

Assembler::ForwardLabel Assembler::call(Register dst) {
	push(Bytecode::Call, dst);
	push_i32(0);
	return label();
}





void Assembler::add(Register dst, Register a, Register b) {
	push(Bytecode::Add, dst, a, b);
}

void Assembler::sub(Register dst, Register a, Register b) {
	push(Bytecode::Sub, dst, a, b);
}

void Assembler::mul(Register dst, Register a, Register b) {
	push(Bytecode::Mul, dst, a, b);
}

void Assembler::div(Register dst, Register a, Register b) {
	push(Bytecode::Div, dst, a, b);
}

void Assembler::cpy(Register dst, Register src) {
	push(Bytecode::Cpy, dst, src);
}

void Assembler::set(Register dst, i32 value) {
	push(Bytecode::Set, dst);
	push_i32(value);
}





void Assembler::jlt(Register a, Register b, Label to) {
	push(Bytecode::Jlt, a, b);
	push_i32(to - label() - sizeof(i32));
}

Assembler::ForwardLabel Assembler::jlt(Register a, Register b) {
	push(Bytecode::Jlt, a, b);
	push_i32(0);
	return label();
}


}
