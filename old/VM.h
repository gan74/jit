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
		enum class ErrorType {
			NoError,
			TypeError,
			UnknownOpError
		};

		struct Error {
			ErrorType type = ErrorType::NoError;
			const Instruction* instruction = nullptr;
		};

		VM(Table* env);

		Error eval(const Program& program, Value* ret);


	private:
		Error eval(const Function& function, Value* ret);

		Value& upvalue(UpValue up);

		void push_stack(const Function& function);
		void pop_stack();

		Value* _func_stack = nullptr;
		std::unique_ptr<Value[]> _stack;
		std::vector<Value*> _stack_frames;

		static bool check_type(const Value& value, ValueType type, const Instruction* instruction, Error& err);

};


}

#endif // JIT_EVAL_H
