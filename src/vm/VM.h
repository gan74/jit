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
#ifndef JIT_EVAL_H
#define JIT_EVAL_H

#include "bytecode.h"
#include "Value.h"
#include "Program.h"

#include <memory>

namespace jit {

class VM {

	public:
		VM(Table* env);

		void eval(const Program& program, Value* ret);


	private:
		void eval(const Function& function, Value* ret, u32& ret_count);

		Value& upvalue(UpValue up);
		Table& tab_upvalue(UpValue up);

		void push_stack(u32 size);
		void pop_stack();

		Value* _func_stack = nullptr;
		std::unique_ptr<Value[]> _stack;
		std::vector<Value*> _stack_frames;

		std::vector<Value> _upvalues;

		static void check_type(const Value& value, ValueType type, const Instruction* instruction);
		static void check_params(const Function& function, u32 args, const Instruction* instruction);

};


}

#endif // JIT_EVAL_H
