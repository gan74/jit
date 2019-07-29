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

#include "Value.h"
#include <cassert>

namespace jit {

Value::Value() : integer(0) {
}

Value::Value(double n) : type(ValueType::Number), number(n) {
}

Value::Value(Table* t) : type(ValueType::Table), ptr(t) {
}

Value::Value(std::string* s) : type(ValueType::String), ptr(s) {
}

Value::Value(std::string_view s) : Value(new std::string(s)) {
}

Value::Value(FunctionPtr f) : type(ValueType::ExternalFunction), ptr(reinterpret_cast<void*>(f)) {
}

Value::Value(const Function* f) : type(ValueType::Closure), c_ptr(f) {
}

Value::Value(const Constant& cst) {
	switch(cst.type) {
		case ConstantType::None:
			type = ValueType::None;
		break;

		case ConstantType::String:
		case ConstantType::LongString:
			type = ValueType::String;
			ptr = new std::string(cst.string);
		break;

		case ConstantType::Integer:
			type = ValueType::Number;
			number = cst.integer;
		break;

		case ConstantType::Number:
			type = ValueType::Number;
			number = cst.number;
		break;

		default:
			fatal("Unsupported constant type.");
		break;
	}
}

const char* Value::type_str(ValueType type) {
	static const char* names[] = {"nil", "number", "table", "string", "function", "function"};
	return names[usize(type)];
}

const char* Value::type_str() const {
	return type_str(type);
}

Table& Value::table() {
	assert(type == ValueType::Table);
	return *reinterpret_cast<Table*>(ptr);
}

std::string& Value::string() {
	assert(type == ValueType::String);
	return *reinterpret_cast<std::string*>(ptr);
}

const Table& Value::table() const {
	assert(type == ValueType::Table);
	return *reinterpret_cast<const Table*>(ptr);
}

const std::string& Value::string() const {
	assert(type == ValueType::String);
	return *reinterpret_cast<const std::string*>(ptr);
}

FunctionPtr Value::func() const {
	assert(type == ValueType::ExternalFunction);
	return reinterpret_cast<FunctionPtr>(ptr);
}

const Function& Value::closure() const {
	assert(type == ValueType::Closure);
	return *reinterpret_cast<const Function*>(c_ptr);
}


bool Value::operator==(const Value& value) const {
	if(type != value.type) {
		return false;
	}
	switch(type) {
		case ValueType::None:
			return true;
		case ValueType::Number:
			return number == value.number;

		case ValueType::String:
#warning intern string
			return string() == value.string();

		default:
			return integer == value.integer;
	}
}

Value& Value::operator=(const Value& v) {
	type = v.type;
	integer = v.integer;
	assert(*this == v);
	return *this;
}


}
