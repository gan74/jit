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


#include <cstdio>

#ifdef __WIN32
#define WIN32_ASSEMBLER
#include <windows.h>
#endif


Assembler::Assembler() {
}


void Assembler::dump() const {
	for(u8 b : _bytes) {
		printf("%02x ", b);
	}
	printf("\n");
}

void* Assembler::alloc_compile() const {
#ifdef WIN32_ASSEMBLER
	static constexpr size_t page_size = 4096;
	void* buffer = VirtualAlloc(nullptr, page_size, MEM_COMMIT, PAGE_READWRITE);
	std::memcpy(buffer, _bytes.data(), _bytes.size());
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






void Assembler::push_r_prefix(Register dst) {
	if(dst.is_r()) {
		push(0x41);
	}
}
void Assembler::push_r_prefix(Register dst, Register src) {
	if(src.is_r() || dst.is_r()) {
		push(0x40 | (src.is_r() << 2) | dst.is_r());
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
	} else if(offset < 0x80) {
		push(0x40 | indexes);
		push(offset);
	} else {
		push(0x80 | indexes);
		push_i32(offset);
	}
}



void Assembler::mov(Register dst, Register src) {
	generic_bin_op(0x89, dst, src);
}

void Assembler::mov(Register dst, i32 value) {
	push_r_prefix(dst);
	push(0xb8 | dst.r_index());
	push_i32(value);
}

void Assembler::mov(Register dst, RegisterOffset src) {
	push(0x67);
	push_r_prefix(src.reg(), dst);
	push(0x8b);
	u8 indexes = (dst.r_index() << 3) | src.reg().r_index();
	push_offset(indexes, src.offset());

}


void Assembler::mov(RegisterOffset dst, Register src) {
	push(0x67);
	push_r_prefix(dst.reg(), src);
	push(0x89);
	u8 indexes = (src.r_index() << 3) | dst.reg().r_index();
	push_offset(indexes, dst.offset());
}

void Assembler::add(Register dst, Register src) {
	generic_bin_op(0x01, dst, src);
}

void Assembler::add(Register dst, i32 value) {
	if(dst == regs::eax) {
		push(0x05);
		push_i32(value);
	} else {
		generic_bin_op(0x81, dst, value);
	}
}







