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
#ifndef JIT_EXCEPTIONS_H
#define JIT_EXCEPTIONS_H

#include "bytecode.h"
#include "Value.h"

namespace jit {

struct ExecutionException : public std::exception {
	ExecutionException(const char* what, const Instruction* instr = nullptr) : instruction(instr), _w(what) {
	}

	const char* what() const noexcept override {
		return _w;
	}

	const Instruction* const instruction = nullptr;

	private:
		const char* _w;
};

struct InvalidInstructionException : public ExecutionException {
	InvalidInstructionException(const Instruction* instr) : ExecutionException("Invalid instruction", instr) {
	}
};

struct TypeErrorException : public ExecutionException {
	TypeErrorException(ValueType expected, ValueType actual, const Instruction* instr = nullptr) :
			ExecutionException("Type error", instr),
			expected_type(expected),
			actual_type(actual) {
	}

	const ValueType expected_type;
	const ValueType actual_type;
};

struct InvalidArgCountException : public ExecutionException {
	InvalidArgCountException(u32 expected, u32 actual, const Instruction* instr = nullptr) :
			ExecutionException("Invalid number of arguments", instr),
			expected_number(expected),
			actual_numer(actual) {
	}

	const u32 expected_number;
	const u32 actual_numer;
};


}

#endif // JIT_EXCEPTIONS_H
