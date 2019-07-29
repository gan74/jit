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

#include "VM.h"
#include "Table.h"
#include "library.h"

#include <algorithm>

#include <cassert>

#define CHECK_TYPE(value, type) do { if(check_type(value, type, pc, error)) { return error; } } while(false)
#define CHECK_NUM(value) CHECK_TYPE(value, ValueType::Number)
#define CHECK_TABLE(value) CHECK_TYPE(value, ValueType::Table)
#define CHECK_CLOSURE(value) CHECK_TYPE(value, ValueType::Closure)
#define R(id) _func_stack[current.id]
#define K(id) function.constants[current.id & Instruction::r_mask]
#define RK(id) (current.id & Instruction::max_k ? K(id) : R(id))

namespace jit {

VM::VM(Table* env) : _stack(std::make_unique<Value[]>(1 << 16)) {
	_stack[0] = env;
	_func_stack = _stack.get() + 1;
	_stack_frames.push_back(_stack.get());

	// first item on the stack is env
	assert(_stack_frames[0][0].type == ValueType::Table);
}

bool VM::check_type(const Value& value, ValueType type, const Instruction* instruction, Error& err) {
	if(value.type != type) {
		err.type = ErrorType::TypeError;
		err.instruction = instruction;
		return true;
	}
	return false;
}

void VM::push_stack(const Function& function) {
	_stack_frames.push_back(_func_stack);
	Value* old_stack = _func_stack;
	_func_stack += function.regs;
	std::copy_n(old_stack, function.params, _func_stack);
}

void VM::pop_stack() {
	_stack_frames.pop_back();
}

Value& VM::upvalue(UpValue up) {
	return _stack_frames[_stack_frames.size() - up.stack][up.reg];
}

VM::Error VM::eval(const Program& program, Value* ret) {
	return eval(program.functions.front(), ret);
}

VM::Error VM::eval(const Function& function, Value* ret) {
	Error error;
	for(const Instruction* pc = function.instructions.begin();; ++pc) {
		Instruction current = *pc;
		printf("%s\n", op_name(OpCode(current.opcode)));

		switch(OpCode(current.opcode)) {

			case OpCode::Move:
				R(A) = R(B);
			break;

			case OpCode::Loadk:
				R(A) = function.constants[current.Bx()];
			break;

			/* ... */

			case OpCode::Gettabup: {
				Value& tab = upvalue(function.upvalues[current.C]);
				CHECK_TABLE(tab);
				R(A) = tab.table().get(RK(B));
			} break;

			case OpCode::Gettable:
				CHECK_TABLE(R(C));
				R(A) = R(C).table().get(RK(B));
			break;

			case OpCode::Settabup: {
				Value& tab = upvalue(function.upvalues[current.A]);
				CHECK_TABLE(tab);
				tab.table().get(RK(C)) = RK(B);
			} break;

			/* ... */

			case OpCode::Add:
				CHECK_NUM(RK(B));
				CHECK_NUM(RK(C));
				R(A) = RK(B).number + RK(C).number;
			break;

			case OpCode::Sub:
				CHECK_NUM(RK(B));
				CHECK_NUM(RK(C));
				R(A) = RK(B).number - RK(C).number;
			break;

			/* ... */

			case OpCode::Jmp:
				pc += current.sBx();
				if(current.A) {
					fatal("Unsupported.");
				}
			break;

			case OpCode::Eq:
				if((RK(B) == RK(C)) != current.A) {
					++pc;
				}
			break;

			/* ... */

			case OpCode::Call: {
				if(R(A).type == ValueType::ExternalFunction) {
					printf("c func \n");
					u32 params = current.B ? current.B - 1 : function.regs - current.A - 1;
					if(current.C == 1) {
						R(A).func()(_func_stack + current.A + 1, params);
					} else if(current.C == 2) {
						R(A) = R(A).func()(_func_stack + current.A + 1, params);
					} else {
						fatal("Unsupported.");
					}
				} else {
					CHECK_CLOSURE(R(A));
					const Function* func = R(A).closure();
					if(func->varargs) {
						fatal("Unsupported.");
					}
					Value* ret_addr = current.C ? &R(A) : nullptr;
					push_stack(*func);
					error = eval(*func, ret_addr);
					pop_stack();
					if(error.type != ErrorType::NoError) {
						return error;
					}
				}
			} break;


			/* ... */

			case OpCode::Return:
				if(ret) {
					Value* end = current.B ? (_func_stack + current.A + current.B) : (_func_stack + function.regs - 1);
					std::copy(_func_stack + current.A, end, ret);
				}
				return error;
			break;

			/* ... */

			case OpCode::Closure:
				R(A) = &function.functions[current.Bx()];
			break;

			default:
				error.type = ErrorType::UnknownOpError;
				error.instruction = pc;
				return error;
		}
	}

	return error;
}

}
