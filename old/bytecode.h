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

#ifndef JIT_BYTECODE_H
#define JIT_BYTECODE_H

#include <utils.h>

#include <vector>

namespace jit {

// http://luaforge.net/docman/83/98/ANoFrillsIntroToLua51VMInstructions.pdf
enum class OpCode : i32 {
	Move,			//	A B			R(A) := R(B)
	Loadk,			//	A Bx		R(A) := Kst(Bx)
	Loadkx,			//	A 			R(A) := Kst(extra arg)
	Loadbool,		//	A B C		R(A) := (Bool)B; if (C) pc++
	Loadnil,		//	A B			R(A), R(A+1), ..., R(A+B) := nil
	Getupval,		//	A B			R(A) := UpValue[B]

	Gettabup,		//	A B C		R(A) := UpValue[B][RK(C)]
	Gettable,		//	A B C		R(A) := R(B)[RK(C)]

	Settabup,		//	A B C		UpValue[A][RK(B)] := RK(C)
	Setupval,		//	A B			UpValue[B] := R(A)
	Settable,		//	A B C		R(A)[RK(B)] := RK(C)

	Newtable,		//	A B C		R(A) := {} (size = B, C)

	Self,			//	A B C		R(A+1) := R(B); R(A) := R(B)[RK(C)]

	Add,			//	A B C		R(A) := RK(B) + RK(C)
	Sub,			//	A B C		R(A) := RK(B) - RK(C)
	Mul,			//	A B C		R(A) := RK(B) * RK(C)
	Mod,			//	A B C		R(A) := RK(B) % RK(C)
	Pow,			//	A B C		R(A) := RK(B) ^ RK(C)
	Div,			//	A B C		R(A) := RK(B) / RK(C)
	Idiv,			//	A B C		R(A) := RK(B) // RK(C)
	Band,			//	A B C		R(A) := RK(B) & RK(C)
	Bor,			//	A B C		R(A) := RK(B) | RK(C)
	Bxor,			//	A B C		R(A) := RK(B) ~ RK(C)
	Shl,			//	A B C		R(A) := RK(B) << RK(C)
	Shr,			//	A B C		R(A) := RK(B) >> RK(C)
	Unm,			//	A B			R(A) := -R(B)
	Bnot,			//	A B			R(A) := ~R(B)
	Not,			//	A B			R(A) := not R(B)
	Len,			//	A B			R(A) := length of R(B)

	Concat,			//	A B C		R(A) := R(B).. ... ..R(C)

	Jmp,			//	A sBx		pc+=sBx; if (A) close all upvalues >= R(A - 1)
	Eq,				//	A B C		if ((RK(B) == RK(C)) ~= A) then pc++
	Lt,				//	A B C		if ((RK(B) <  RK(C)) ~= A) then pc++
	Le,				//	A B C		if ((RK(B) <= RK(C)) ~= A) then pc++

	Test,			//	A C			if not (R(A) <=> C) then pc++
	Testset,		//	A B C		if (R(B) <=> C) then R(A) := R(B) else pc++

	Call,			//	A B C		R(A), ... , R(A+C-2) := R(A)(R(A+1), ... , R(A+B-1))
	Tailcall,		//	A B C		return R(A)(R(A+1), ... , R(A+B-1))
	Return,			//	A B			return R(A), ... , R(A+B-2)	(see note)

	Forloop,		//	A sBx		R(A)+=R(A+2); if R(A) <?= R(A+1) then { pc+=sBx; R(A+3)=R(A) }
	Forprep,		//	A sBx		R(A)-=R(A+2); pc+=sBx

	Tforcall,		//	A C			R(A+3), ... , R(A+2+C) := R(A)(R(A+1), R(A+2));
	Tforloop,		//	A sBx		if R(A+1) ~= nil then { R(A)=R(A+1); pc += sBx }

	Setlist,		//	A B C		R(A)[(C-1)*FPF+i] := R(A+i), 1 <= i <= B

	Closure,		//	A Bx		R(A) := closure(KPROTO[Bx])

	Vararg,			//	A B			R(A), R(A+1), ..., R(A+B-2) = vararg

	Extraarg		//	Ax			extra (larger) argument for previous opcode
};


const char* op_name(OpCode op);

// https://the-ravi-programming-language.readthedocs.io/en/latest/lua_bytecode_reference.html
struct Instruction {
	static constexpr i32 max_k = 1 << 8;
	static constexpr u32 r_mask = u32(max_k) - 1;
	static constexpr i32 max_bx = 1 << 17;

	u32 opcode : 6;
	u32 A : 8;
	u32 B : 9;
	u32 C : 9;

	u32 Bx() const {
		return (C << 9) | B;
	}

	i32 sBx() const {
		return i32(Bx()) - max_bx;
	}
};

static_assert(sizeof(Instruction) == sizeof(u32));



enum class ConstantType : u8 {
	None = 0x00,
	String = 0x04,
	Int = 0x13,
	Number = 0x03,
};

struct Constant {
	ConstantType type = ConstantType::None;

	union {
		std::string_view string;
		i64 integer;
		double number;
	};

	Constant() {
	}
};

static_assert(std::is_trivially_destructible_v<Constant>);

struct UpValue {
	u8 stack;
	u8 reg;
};

static_assert(sizeof(UpValue) == sizeof(u16));

struct Function {
	ArrayView<Instruction> instructions;
	std::vector<Constant> constants;
	ArrayView<UpValue> upvalues;
	std::vector<Function> functions;

	u32 params = 0;
	u32 varargs = 0;
	u32 regs = 0;

	std::string_view info;
	std::string_view src;
	ArrayView<u32> lines;
};


}

#endif // JIT_BYTECODE_H
