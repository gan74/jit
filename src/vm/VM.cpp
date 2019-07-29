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
#include "exceptions.h"

#include <algorithm>

#include <cmath>

#define CHECK_PARAMS(func, args) check_params(func, args)
#define CHECK_TYPE(value, type) check_type(value, type)
#define CHECK_NUM(value) CHECK_TYPE(value, ValueType::Number)
#define CHECK_TABLE(value) CHECK_TYPE(value, ValueType::Table)
#define CHECK_CLOSURE(value) CHECK_TYPE(value, ValueType::Closure)
#define R(id) _func_stack[current.id]
#define K(id) function.constants[current.id & Instruction::r_mask]
#define RK(id) (current.id & Instruction::max_k ? K(id) : R(id))
#define UP(id) (function.upvalues[current.id])

namespace jit {

VM::VM(Table* env) : _stack(std::make_unique<Value[]>(1 << 16)) {
	_stack[0] = env;
	_upvalues.push_back(env);

	_stack_frames.push_back(_stack.get());
	_func_stack = _stack.get() + 1;
}

void VM::check_params(const Function& function, u32 args) {
	if(function.varargs || function.params != args) {
		throw InvalidArgCountException(function.params, args);
	}
}

void VM::check_type(const Value& value, ValueType type) {
	if(value.type != type) {
		throw TypeErrorException(type, value.type);
	}
}

void VM::push_stack(u32 size) {
	_stack_frames.push_back(_func_stack);
	_func_stack += size;
}

void VM::pop_stack() {
	assert(!_stack_frames.empty());
	_func_stack = _stack_frames.back();
	_stack_frames.pop_back();
}

Value& VM::upvalue(UpValue up) {
	if(up.stack) {
		assert(up.stack <= _stack_frames.size());
		Value* stack = _stack_frames[_stack_frames.size() - up.stack];
		return stack[up.reg];
	}
	assert(up.reg < _upvalues.size());
	return _upvalues[up.reg];
}


Table& VM::tab_upvalue(UpValue up) {
	Value& val = upvalue(up);
	if(val.type == ValueType::Table) {
		return val.table();
	}
	assert(val.type == ValueType::None);
	Table* t = new Table();
	val = t;
	return *t;
}

void VM::eval(const Program& program, Value* ret) {
	u32 rets = 1;
	return eval(program.functions.front(), ret, rets);
}

void VM::eval(const Function& function, Value* ret, u32& ret_count) {
	// MutableSpan<Value> outs = C == 1 ? MutableSpan<Value>() : MutableSpan<Value>(_func_stack + A, C - 1);
	auto call = [this, &function](const Value& func_val, MutableSpan<Value> out, Span<Value> in) -> u32 {
		if(func_val.type == ValueType::ExternalFunction) {
			return func_val.func()(out, in);
		}
		CHECK_CLOSURE(func_val);
		const Function& func = func_val.closure();
		CHECK_PARAMS(func, in.size());
		push_stack(function.regs);
		std::copy(in.begin(), in.end(), _func_stack);
		u32 returned = out.size();
		eval(func, out.begin(), returned);
		pop_stack();
		return returned;
	};


	const u32 max_args = 254;
	u32 last_ret_count = 0;
	const Instruction* pc = nullptr;
	try {
		for(pc = function.instructions.begin();; ++pc) {
			Instruction current = *pc;

			//std::printf("%s %u %u %u\n", op_name(OpCode(current.opcode)), current.A, current.B, current.C);
			//std::printf("%s\n", op_name(OpCode(current.opcode)));
			/*

			for(u32 i = 0; i != function.regs + 5; ++i) {
				std::printf("[%d] ", i);
				lib::print(_func_stack[i]);
			}

			*/

			switch(OpCode(current.opcode)) {

				case OpCode::Move:
					R(A) = R(B);
				break;

				case OpCode::Loadk:
					R(A) = function.constants[current.Bx()];
				break;

				/* ... */

				case OpCode::Getupval:
					R(A) = upvalue(UP(B));
				break;

				case OpCode::Gettabup: {
					Value& tab = upvalue(UP(B));
					CHECK_TABLE(tab);
					R(A) = tab.table().get(RK(C));
				} break;

				case OpCode::Gettable:
					CHECK_TABLE(R(B));
					R(A) = R(B).table().get(RK(C));
				break;

				case OpCode::Settabup: {
					Table& tab = tab_upvalue(UP(A));
					tab.get(RK(B)) = RK(C);
				} break;

				case OpCode::Setupval:
					upvalue(UP(B)) = R(A);
				break;

				/* ... */

				case OpCode::Newtable:
	#warning use B and C
					R(A) = new Table();
				break;

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

				case OpCode::Mul:
					CHECK_NUM(RK(B));
					CHECK_NUM(RK(C));
					R(A) = RK(B).number * RK(C).number;
				break;

				case OpCode::Mod:
					CHECK_NUM(RK(B));
					CHECK_NUM(RK(C));
					R(A) = std::fmod(RK(B).number, RK(C).number);
				break;

				case OpCode::Pow:
					CHECK_NUM(RK(B));
					CHECK_NUM(RK(C));
					R(A) = std::pow(RK(B).number, RK(C).number);
				break;

				case OpCode::Div:
					CHECK_NUM(RK(B));
					CHECK_NUM(RK(C));
					R(A) = RK(B).number / RK(C).number;
				break;

				/* ... */

				case OpCode::Jmp:
					pc += current.sBx() + 1;
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
					u32 returns = current.C ? current.C - 1 : max_args;
					MutableSpan<Value> out(_func_stack + current.A, returns);

					u32 args = current.B ? current.B - 1 : last_ret_count;
					Span<Value> in(_func_stack + current.A + 1, args);

					last_ret_count = call(R(A), out, in);
				} break;


				/* ... */

				case OpCode::Return:
					if(ret) {
						ret_count = std::min(ret_count, current.B ? current.B - 1 : function.regs - current.A);
						std::copy_n(_func_stack + current.A, ret_count, ret);
						/*printf("ret = %d\n", ret_count);
						lib::print(ret, ret_count);*/
					}
					return;
				break;

				case OpCode::Forloop:
					CHECK_NUM(R(A));
					CHECK_NUM(R(A + 1));
					CHECK_NUM(R(A + 2));
					R(A).number += R(A + 2).number;
					if(R(A + 2).number > 0.0 ? R(A).number <= R(A + 1).number : R(A).number >= R(A + 1).number) {
						pc += current.sBx() + 1;
						R(A + 3) = R(A);
					}
				break;

				case OpCode::Forprep:
					CHECK_NUM(R(A));
					CHECK_NUM(R(A + 2));
					R(A).number -= R(A + 2).number;
					pc += current.sBx() + 1;
				break;

				case OpCode::Tforcall: {
					if(!current.C) {
						fatal("Unsupported.");
					}
					MutableSpan<Value> out(_func_stack + current.A + 3, current.C/* - 1*/);
					Span<Value> in(_func_stack + current.A + 1, 2);

					call(R(A), out, in);
				} break;

				case OpCode::Tforloop:
					if(R(A + 1) != Value()) {
						R(A) = R(A + 1);
						pc += current.sBx() + 1;
					}
				break;


				/* ... */

				case OpCode::Closure:
					R(A) = &function.functions[current.Bx()];
				break;

				default:
					throw InvalidInstructionException(pc);
			}
		}
	} catch(ExecutionException& exception) {
		if(!exception.instruction) {
			exception.instruction = pc;
		}
		throw;
	}
}

}
