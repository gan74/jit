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

#include "library.h"

#include "exceptions.h"

#include <cmath>
#include <cstdio>

namespace jit {
namespace lib {

static void check_params(usize expected, usize len) {
	if(len != expected) {
		throw InvalidArgCountException(u32(expected), u32(len));
	}
}

static void check_type(const Value& v, ValueType expected) {
	if(v.type != expected) {
		throw TypeErrorException(expected, v.type);
	}
}


static Table* default_io() {
	Table* io = new Table();

	io->get(Value("read")) = &io_read;

	return io;
}

static Table* default_math() {
	Table* io = new Table();

	io->get(Value("sqrt")) = &math_sqrt;

	return io;
}

static Table* default_table() {
	Table* io = new Table();

	io->get(Value("insert")) = &table_insert;

	return io;
}

Table default_env() {
	Table env;

	env[Value("print")] = &print;
	env[Value("math")] = default_math();
	env[Value("io")] = default_io();
	env[Value("table")] = default_table();

	return env;
}

Value print(const Value& v) {
	return print(const_cast<Value*>(&v), 1);
}

Value print(Value* v, usize len) {
	for(usize i = 0; i != len; ++i) {
		switch(v[i].type) {
			case ValueType::None:
				std::printf("nil ");
			break;

			case ValueType::Number:
				std::printf("%lf ", v[i].number);
			break;

			case ValueType::String:
				std::printf("%s ", v[i].string().data());
			break;

			default:
				std::printf("%s: %p ", v[i].type_str(), v[i].ptr);
		}
	}
	std::printf("\n");
	return Value();
}

Value math_sqrt(Value* v, usize len) {
	check_params(1, len);
	check_type(v[0], ValueType::Number);

	return std::sqrt(v->number);
}

Value io_read(Value* v, usize len) {
	if(len) {
		if(len != 1 || v->type != ValueType::String || v->string() != "*number") {
			print(*v);
			fatal("Unsupported");
		}
	}

	double d = 0.0;
	std::scanf("%lf", &d);

	return d;
}


Value table_insert(Value* v, usize len) {
	check_params(2, len);
	check_type(v[0], ValueType::Table);

	Table& t = v[0].table();
	t.get(Value(t.size() + 1)) = v[1];

	return Value();
}

}
}
