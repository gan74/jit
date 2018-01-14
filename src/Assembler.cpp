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


Assembler::Assembler() {
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
	// retq
	push(0xc3);
}



void Assembler::mov_eax(i32 value) {
	// mov eax value
	push(0xb8);
	push_i32(value);
}

void Assembler::add_eax(i32 value) {
	// add eax,value
	push(0x05);
	push_i32(value);
}



void Assembler::arg_to_eax(u8 arg_index) {
	switch(arg_index) {
		case 0:
			// mov %eax,%ecx
			push(0x89, 0xc8);
			break;

		case 1:
			// mov %eax,%edx
			push(0x89, 0xd0);
			break;

		case 2:
			// mov %eax, %r8d
			push(0x44, 0x89, 0xc0);
			break;

		case 3:
			// mov %eax, %r9d
			push(0x44, 0x89, 0xc8);
			break;

		default:
			// mov %eax, arg_offset(%rbp)
			u16 arg_offset = 0x10 + arg_index * 8;
			if(arg_offset < 0x80) {
				push(0x8b, 0x45, arg_offset);
			} else {
				push(0x8b, 0x85);
				push_i32(arg_offset);
			}
			break;
	}

}
