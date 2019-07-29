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
#ifndef JIT_VALUE_H
#define JIT_VALUE_H

#include <utils.h>

#include "bytecode.h"

namespace jit {

class Table;

enum class ValueType {
	None,
	Number,
	Table,
	String,

	Closure,
	ExternalFunction
};

struct Value;
using FunctionPtr = Value(*)(Value*, usize);

struct Value {
	ValueType type = ValueType::None;

	union {
		i64 integer;
		double number;
		void* ptr;
		const void* c_ptr;
	};

	Value();

	Value(double n);
	Value(Table* t);
#warning string should be able to contain nulls
	Value(std::string* s);
	Value(std::string_view s);
	Value(FunctionPtr f);
	Value(const Function* f);

	Value(const Constant& cst);

	static const char* type_str(ValueType type);
	const char* type_str() const;

	Table& table();
	std::string& string();

	const Table& table() const;
	const std::string& string() const;
	FunctionPtr func() const;
	const Function& closure() const;


	bool operator==(const Value& value) const;

	Value& operator=(const Value& v);

};



}

#endif // VALUE_H
