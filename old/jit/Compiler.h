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
#ifndef JIT_COMPILER_H
#define JIT_COMPILER_H

#include <ast/ast.h>

#include "Assembler.h"

namespace jit {

class Compiler {

	public:
		Compiler();

		void compile(const Ast& ast);

		const Assembler& assembler() const;

	private:
		void compile_rhs(Register dst, const Node& node, const Ast& ast);
		void compile_rhs(RegisterIndexOffsetRegister dst, const Node& node, const Ast& ast);

		Register compile_lhs(const Node& node, const Ast& ast);


		Assembler _assembler;
		std::vector<RegisterIndexOffsetRegister> _symbols;
		std::vector<Register> _free_regs;
};

}

#endif // JIT_COMPILER_H
