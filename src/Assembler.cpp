/*******************************
Copyright (c) 2016-2017 Gr�goire Angerand

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
#include "Assembler.h"


#ifdef __WIN32
#define WIN32_ASSEMBLER
#include <windows.h>
#endif

void check_bits(Register a, u8 bits) {
	if(a.bits() != bits) {
		fatal("Invalid number of bits");
	}
}

void check_bits(Register a, Register b) {
	check_bits(a, b.bits());
}


Assembler::Assembler() {
}

void* Assembler::alloc_compile() const {
#ifdef WIN32_ASSEMBLER
	static constexpr size_t page_size = 4096;

	void* buffer = VirtualAlloc(nullptr, page_size, MEM_COMMIT, PAGE_READWRITE);
	std::memcpy(buffer, _bytes.data(), _bytes.size());

	for(const auto& call : _calls) {
		u8* addr = reinterpret_cast<u8*>(buffer) + call.first._index;
		auto diff = call.second - addr;
		*reinterpret_cast<u32*>(addr) = u32(diff) - 4; // wat ?
	}

	DWORD res = 0;
	VirtualProtect(buffer, _bytes.size(), PAGE_EXECUTE_READ, &res);
	return buffer;
#else
#error unsupported OS.
#endif
}





void Assembler::push_stack() {
	// push   %rbp
	push(0x55);
	// mov    %rsp,%rbp
	push(0x48, 0x89, 0xe5);
}

void Assembler::pop_stack() {
	// pop    %rbp
	push(0x5d);
}

void Assembler::ret() {
	push(0xc3);
}

void Assembler::nop() {
	push(0x90);
}



void Assembler::forward_label(u32 label) {
	u32 diff =  _bytes.size() - label - 4;
	*reinterpret_cast<u32*>(&_bytes[label]) = diff;
}

void Assembler::push_r_prefix(Register dst) {
	if(dst.is_r() || dst.is_64()) {
		push(0x40 | (dst.is_64() << 3) | dst.is_r());
	}
}
void Assembler::push_r_prefix(Register a, Register b) {
	bool is_64 = b.is_64();
	if(a.is_r() || b.is_r() || is_64) {
		u8 opcode = 0x40 | (is_64 << 3);
		push(opcode | (b.is_r() << 2) | a.is_r());
	}
}

void Assembler::push_r_prefix_index(Register a, Register b) {
	bool is_64 = b.is_64();
	if(a.is_r() || b.is_r() || is_64) {
		u8 opcode = 0x40 | (is_64 << 3);
		push(opcode | (b.is_r() << 2) | (a.is_r() << 1));
	}
}

void Assembler::generic_bin_op(u8 opcode, Register dst, Register src) {
	push_r_prefix(dst, src);
	push(opcode, 0xc0 | (src.r_index() << 3) | dst.r_index());

}
void Assembler::generic_bin_op(u8 opcode, Register dst, i32 value) {
	push_r_prefix(dst);
	push(opcode, 0xc0 | dst.r_index());
	push_i32(value);
}

void Assembler::push_offset(u8 indexes, u32 offset) {
	if (!offset) {
		push(indexes);
	} else if(is_8_bits(offset)) {
		push(0x40 | indexes);
		push(offset);
	} else {
		push(0x80 | indexes);
		push_i32(offset);
	}
}




void Assembler::mov(Register dst, Register src) {
	// 64 bits = ok
	check_bits(dst, src);
	generic_bin_op(0x89, dst, src);
}

void Assembler::mov(Register dst, i32 value) {
	// 64 bits = ok
	if(dst.is_64() && !dst.is_r()) {
		push(0x48);
	}
	push_r_prefix(dst);
	push(0xb8 | dst.r_index());
	push_i32(value);
	// TODO: use instruction for 32 bit payloads
	if(dst.is_64()) {
		push_i32(0);
	}
}

void Assembler::mov(Register dst, RegisterOffset src) {
	// 64 bits = ok
	if(!src.reg().is_64()) {
		push(0x67);
	}
	push_r_prefix(src.reg(), dst);
	push(0x8b);
	u8 indexes = (dst.r_index() << 3) | src.reg().r_index();
	push_offset(indexes, src.offset());

}

void Assembler::mov(RegisterOffset dst, Register src) {
	// 64 bits = ok
	if(!dst.reg().is_64()) {
		push(0x67);
	}
	push_r_prefix(dst.reg(), src);
	push(0x89);
	u8 indexes = (src.r_index() << 3) | dst.reg().r_index();
	push_offset(indexes, dst.offset());
}

void Assembler::mov(Register dst, RegisterIndexOffset src) {
	// 64 bits = ok
	if(!src.reg().is_64()) {
		push(0x67);
	}
	push_r_prefix_index(src.reg(), dst);
	/*{
		bool is_64 = dst.is_64();
		if(src.reg().is_r() || dst.is_r() || is_64) {
			u8 opcode = 0x40 | (is_64 << 3);
			push(opcode | (dst.is_r() << 2) | (src.reg().is_r() << 1));
		}
	}*/
	push(0x8b, 0x04 | (dst.r_index() << 3), 0x05 | (src.size() << 5) | (src.reg().r_index() << 3));
	push_i32(src.offset());
}

void Assembler::mov(RegisterIndexOffset dst, Register src) {
	// 64 bits = ok
	if(!dst.reg().is_64()) {
		push(0x67);
	}
	push_r_prefix_index(dst.reg(), src);
	/*{
		bool is_64 = src.is_64();
		if(dst.reg().is_r() || src.is_r() || is_64) {
			u8 opcode = 0x40 | (is_64 << 3);
			push(opcode | (src.is_r() << 2) | (dst.reg().is_r() << 1));
		}
	}*/
	push(0x89, 0x04 | (src.r_index() << 3), 0x05 | (dst.size() << 5) | (dst.reg().r_index() << 3));
	push_i32(dst.offset());
}

void Assembler::mov(Register dst, RegisterIndexOffsetRegister src) {
	// 64 bits = ok
	if(!src.reg().is_64()) {
		push(0x67);
	}
	//push_r_prefix_index_offset(src.reg(), dst);
	{
		bool is_64 = dst.is_64();
		if(src.reg().is_r() || dst.is_r() || is_64) {
			u8 opcode = 0x40 | (is_64 << 3);
			push(opcode | (dst.is_r() << 2) | (src.reg().is_r() << 1) | src.offset().reg().is_r());
		}
	}
	bool fixed_offset = src.has_fixed_offset();
	push(0x8b, 0x04 | (fixed_offset << 6) | (dst.r_index() << 3));
	push((src.size() << 5) | (src.reg().r_index() << 3) | src.offset().reg().r_index());
	if(fixed_offset) {
		push(src.fixed_offset());
	}
}

void Assembler::mov(RegisterIndexOffsetRegister dst, Register src) {
	// 64 bits = ok
	if(!dst.reg().is_64()) {
		push(0x67);
	}
	//push_r_prefix_index_offset(src.reg(), dst);
	{
		bool is_64 = src.is_64();
		if(dst.reg().is_r() || src.is_r() || is_64) {
			u8 opcode = 0x40 | (is_64 << 3);
			push(opcode | (src.is_r() << 2) | (dst.reg().is_r() << 1) | dst.offset().reg().is_r());
		}
	}
	bool fixed_offset = dst.has_fixed_offset();
	push(0x89, 0x04 | (fixed_offset << 6) | (src.r_index() << 3));
	push((dst.size() << 5) | (dst.reg().r_index() << 3) | dst.offset().reg().r_index());
	if(fixed_offset) {
		push(dst.fixed_offset());
	}
}




void Assembler::add(Register dst, Register src) {
	// 64 bits = ok
	check_bits(dst, src);
	generic_bin_op(0x01, dst, src);
}

void Assembler::add(Register dst, i32 value) {
	// 64 bits = ok
	if(dst == regs::rax) {
		push(0x48, 0x05);
		push_i32(value);
	} else if(dst == regs::eax) {
		push(0x05);
		push_i32(value);
	} else {
		generic_bin_op(0x81, dst, value);
	}
}




void Assembler::cmp(Register a, Register b) {
	generic_bin_op(0x39, a, b);
}




void Assembler::je(Label to) {
	push(0x0f, 0x84);
	push_i32(to - label() - 4);
}

Assembler::ForwardLabel Assembler::je() {
	push(0x0f, 0x84);
	ForwardLabel l = label();
	push_i32(0);
	return l;
}




void Assembler::jne(Label to) {
	push(0x0f, 0x85);
	push_i32(to - label() - 4);
}

Assembler::ForwardLabel Assembler::jne() {
	push(0x0f, 0x85);
	ForwardLabel l = label();
	push_i32(0);
	return l;
}



void Assembler::call(Register fn) {
	check_bits(fn, 64);
	if(fn.is_r()) {
		push(0x41);
	}
	push(0xff, 0xd0 | fn.r_index());
}

void Assembler::call(void* fn_ptr) {
	push(0xe8);
	_calls.push_back({label(), reinterpret_cast<u8*>(fn_ptr)});
	push_i32(0);
}




Assembler::Label Assembler::label() const {
	return Label(_bytes.size());
}



