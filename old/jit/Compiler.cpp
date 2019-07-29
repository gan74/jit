/*******************************
Copyright (c) 2016-2018 Gr�goire Angerand

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

#include "Compiler.h"

namespace jit {

Compiler::Compiler() {
	_free_regs.push_back(regs::rax);
	_free_regs.push_back(regs::rbx);
	_free_regs.push_back(regs::r10);
	_free_regs.push_back(regs::r11);
	_free_regs.push_back(regs::r12);
	_free_regs.push_back(regs::r13);
	_free_regs.push_back(regs::r14);
	_free_regs.push_back(regs::r15);
}

void Compiler::compile(const Ast& ast) {
	compile_rhs(regs::rax , ast.root(), ast);
}

void Compiler::compile_rhs(Register dst, const Node& node, const Ast& ast) {
	switch(node.type) {
		case NodeType::NumLit:
			_assembler.mov(dst, node.data.lit.value);
		break;

		case NodeType::Symbol:
			_assembler.mov(dst, _symbols[node.data.sym.index]);
		break;

		default:
			fatal("Unsupported.");
	}
}

void Compiler::compile_rhs(RegisterIndexOffsetRegister dst, const Node& node, const Ast& ast) {
	switch(node.type) {
		default:
			fatal("Unsupported.");
	}
}


Register Compiler::compile_lhs(const Node& node, const Ast& ast) {
	switch(node.type) {
		default:
			fatal("Unsupported.");
	}
}

const Assembler& Compiler::assembler() const {
	return _assembler;
}

}
