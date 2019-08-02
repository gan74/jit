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
#include <algorithm>
#include <chrono>

namespace jit {
namespace lib {

static void check_params(usize expected, usize len) {
	if(len != expected) {
		throw InvalidArgCountException(u32(expected), u32(len));
	}
}

static void check_params(usize expected, Span<Value> args) {
	check_params(expected, args.size());
}

static void check_type(const Value& v, ValueType expected) {
	if(v.type != expected) {
		throw TypeErrorException(expected, v.type);
	}
}

template<typename T, typename... Args>
[[nodiscard]] static u32 build_out(MutableSpan<Value> out, T&& t, Args&&... args) {
	if(out.size()) {
		out[0] = std::forward<decltype(t)>(t);
		if constexpr(sizeof...(args)) {
			return 1 + build_out({out.data() + 1, out.size() - 1}, std::forward<decltype(args)>(args)...);
		}
		return 1;
	}
	return 0;
}


static Table* default_io() {
	Table* t = new Table();

	t->set(Value("read"),  &io_read);

	return t;
}

static Table* default_math() {
	Table* t = new Table();

	t->set(Value("sqrt"), &math_sqrt);

	return t;
}

static Table* default_table() {
	Table* t = new Table();

	t->set(Value("insert"), &table_insert);

	return t;
}

static Table* default_os() {
	Table* t = new Table();

	t->set(Value("clock"), &os_clock);

	return t;
}

Table default_env() {
	Table env;

	env.set(Value("print"), &print);
	env.set(Value("tonumber"), &to_number);
	env.set(Value("pairs"), &pairs);
	env.set(Value("ipairs"), &ipairs);

	env.set(Value("math"), default_math());
	env.set(Value("io"), default_io());
	env.set(Value("table"), default_table());
	env.set(Value("os"), default_os());

	return env;
}

void print(const Value& v) {
	print({}, v);
}




u32 print(MutableSpan<Value>, Span<Value> in) {
	for(const Value& v : in) {
		switch(v.type) {
			case ValueType::None:
				std::printf("nil ");
			break;

			// See https://stackoverflow.com/questions/277772/avoid-trailing-zeroes-in-printf
			case ValueType::Number: {
				double f = v.number;
				char buffer[64];
				int max_decimals = 10;
				double order = std::pow(10, max_decimals);
				double frac = std::abs(f) - std::abs(int(f));
				assert(frac >= 0.0);
				std::sprintf(buffer,"%.*g", max_decimals, std::ceil(order * frac) / order);
				std::printf("%d%s ", int(f), buffer + 1);
			} break;

			case ValueType::Bool:
				std::printf(v.integer ? "true " : "false ");
			break;

			case ValueType::String:
				std::printf("%s ", v.string().data());
			break;

			default:
				std::printf("%s: %p ", v.type_str(), v.ptr);
		}
	}
	std::printf("\n");

	return 0;
}

u32 to_number(MutableSpan<Value> out, Span<Value> in) {
	check_params(1, in);

	if(in[0].type == ValueType::String) {
		const char* str = in[0].string().c_str();
		char* end = nullptr;
		double d = std::strtod(str, &end);
		if(end == str) {
			return build_out(out, Value());
		}
		return build_out(out, d);
	}
	if(in[0].type == ValueType::Number) {
		return build_out(out, in[0]);
	}
	return build_out(out, Value());

}

u32 pairs(MutableSpan<Value> out, Span<Value> in) {
	check_params(1, in);

	/*unused(out);
	fatal("Unsupported.");
	return 0;*/

	FunctionPtr iterate = [](MutableSpan<Value> out, Span<Value> in) -> u32 {
		check_params(2, in);
		check_type(in[0], ValueType::Table);

		Table& table = in[0].table();
		auto it = in[1].type == ValueType::None
			? table.begin()
			: std::find_if(table.begin(), table.end(), [&](const auto& p) { return p.first == in[1]; }) + 1;

		if(it == table.end()) {
			return build_out(out, Value());
		}

		return build_out(out, it->first, it->second);
	};

	return build_out(out, iterate, in[0], Value());
}

u32 ipairs(MutableSpan<Value> out, Span<Value> in) {
	check_params(1, in);

	FunctionPtr iterate = [](MutableSpan<Value> out, Span<Value> in) -> u32 {
		check_params(2, in);
		check_type(in[0], ValueType::Table);
		check_type(in[1], ValueType::Number);

		Table& table = in[0].table();
		double next = in[1].number + 1.0;

		if(next >= table.size()) {
			return build_out(out, Value());
		}
		return build_out(out, next, table.get(next));
	};

	return build_out(out, iterate, in[0], 0);
}

u32 math_sqrt(MutableSpan<Value> out, Span<Value> in) {
	check_params(1, in);
	check_type(in[0], ValueType::Number);

	return build_out(out, std::sqrt(in[0].number));
}

u32 io_read(MutableSpan<Value> out, Span<Value> in) {
	if(in.size()) {
		if(in.size() != 1 || in[0].type != ValueType::String || in[0].string() != "*number") {
			print(in[0]);
			fatal("Unsupported");
		}
	}

	double d = 0.0;
	std::scanf("%lf", &d);

	return build_out(out, d);
}

u32 table_insert(MutableSpan<Value>, Span<Value> in) {
	check_params(2, in);
	check_type(in[0], ValueType::Table);

	Table& t = in[0].table();
	t.set(Value(t.size() + 1), in[1]);

	return 0;
}

u32 os_clock(MutableSpan<Value> out, Span<Value> in) {
	check_params(0, in);
	static auto start = std::chrono::high_resolution_clock::now();

	auto now = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> secs = now - start;
	return build_out(out, secs.count());
}


}
}
