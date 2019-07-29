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

#include "Program.h"

#include "Value.h"

namespace jit {

using Integer = decltype(Constant::integer);
using Number = decltype(Constant::number);

#define READ(type) [&]() -> type { type v = *reinterpret_cast<const type*>(data + len); len += sizeof(type); return v; }()


static std::string_view parse_string(const u8* data, usize& len) {
	u8 byte_size = READ(u8);
	if(!byte_size) {
		return std::string_view();
	}
	usize size = byte_size;
	if(byte_size == 0xFF) {
		size = READ(usize);
	}
	--size; // ??
	std::string_view str(reinterpret_cast<const char*>(data + len), size);
	len += size;
	return str;
}

static Function parse_func(const u8* data, usize& len) {
	Function func;
	func.info = parse_string(data, len);

	u32 start_line = READ(u32);
	u32 end_line = READ(u32);
	unused(start_line, end_line);

	func.params = READ(u8);
	func.varargs = READ(u8);
	func.regs = READ(u8);

	u32 code_size = READ(u32);
	func.instructions = ArrayView<Instruction>(reinterpret_cast<const Instruction*>(data + len), code_size);
	len += code_size * sizeof(Instruction);

	u32 constants = READ(u32);
	func.constants.reserve(constants);
	for(u32 i = 0; i != constants; ++i) {
		Constant& cst = func.constants.emplace_back();
		switch(cst.type = ConstantType(READ(u8))) {
			case ConstantType::None:
			break;

			case ConstantType::String:
			case ConstantType::LongString:
				cst.string = parse_string(data, len);
			break;

			case ConstantType::Integer:
				cst.integer = READ(Integer);
			break;

			case ConstantType::Number:
				cst.number = READ(Number);
			break;

			default:
				fatal("Unsupported constant type.");
		}
	}

	u32 upvalues = READ(u32);
	func.upvalues = ArrayView<UpValue>(reinterpret_cast<const UpValue*>(data + len), upvalues);
	len += upvalues * sizeof(UpValue);

	u32 prototypes = READ(u32);
	func.functions.reserve(prototypes);
	for(u32 i = 0; i != prototypes; ++i) {
		func.functions.emplace_back(parse_func(data, len));
	}

	// debug
	{
		u32 line_nums = READ(u32);
		func.lines = ArrayView<u32>(reinterpret_cast<const u32*>(data + len), line_nums);
		len += line_nums * sizeof(u32);

		u32 local_names = READ(u32);
		for(u32 i = 0; i != local_names; ++i) {
			std::string_view name = parse_string(data, len);
			unused(name);
			len += 2 * sizeof(u32); // skip scope bounds
		}

		u32 upvalue_names = READ(u32);
		for(u32 i = 0; i != upvalue_names; ++i) {
			std::string_view name = parse_string(data, len);
			unused(name);
		}
	}

	return func;
}

static void print_function(const Function& func) {
	printf("bytecode for <%.*s>:\n", i32(func.info.size()), func.info.data());
	for(const auto& i : func.instructions) {
		auto make_k = [](u32 r) { return r < Instruction::max_k ? i32(r) : Instruction::max_k - i32(r) - 1; };
		printf("\t%9s   %d %d %d\n", op_name(OpCode(i.opcode)), i.A, make_k(i.B), make_k(i.C));
	}

	printf("constants for <%.*s>:\n", i32(func.info.size()), func.info.data());
	for(const auto& c : func.constants) {
		switch(c.type) {
			case ConstantType::String:
			case ConstantType::LongString:
				printf("\t\"%.*s\"\n", i32(c.string.size()), c.string.begin());
			break;

			case ConstantType::Number:
				printf("\t%f\n", c.number);
			break;

			case ConstantType::Integer:
				printf("\t%lld\n", c.integer);
			break;

			default:
				fatal("Unsupported constant type.");
		}
	}
	printf("upvalues for <%.*s>:\n", i32(func.info.size()), func.info.data());
	for(UpValue v : func.upvalues) {
		printf("\t%d\t%d\n", v.stack, v.reg);
	}

	printf("\n");

	for(const auto& f : func.functions) {
		print_function(f);
	}
}

Program Program::from_luac(ArrayView<u8> luac_data) {
	usize len = 0;
	const u8* data = luac_data.data();

	if(READ(u32) != 0x61754c1b) {
		fatal("Invalid luac signature.");
	}

	if(READ(u16) == 0x5300) {
		fatal("Invalid luac version.");
	}

	if(READ(u16) != 0x9319 || READ(u16) != 0x0a0d || READ(u16) != 0x0a1a) {
		fatal("Invalid luac check.");
	}

	if(READ(u8) != sizeof(i32) ||
	   READ(u8) != sizeof(usize) ||
	   READ(u8) != sizeof(Instruction) ||
	   READ(u8) != sizeof(Integer) ||
	   READ(u8) != sizeof(Number)) {

		fatal("Invalid luac properties.");
	}

	if(READ(Integer)!= 0x5678) {
		fatal("Invalid luac int check.");
	}

	if(READ(Number) != 370.5) {
		fatal("Invalid luac double check.");
	}

	u8 upvalues = READ(u8);

	Program program;
	while(len < luac_data.size()) {
		program.functions.emplace_back(parse_func(data, len));
	}

	/*for(const auto& f : program.functions) {
		print_function(f);
	}*/

	return program;
}

}
