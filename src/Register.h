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
#ifndef REGISTER_H
#define REGISTER_H

#include "utils.h"

// mainly from https://github.com/t0rakka/realtime/blob/master/source/register.hpp

class Register {
	public:
		explicit Register(u32 index, u8 bits = 32) : _data(index + (bits << 4)) {
			if (bits != 32 && bits != 64) {
				fatal("Unsupported");
			}
		}


		u8 bits() const {
			return _data >> 4;
		}

		u32 index() const {
			return _data & 0xf;
		}

		u32 r_index() const {
			return _data & 0x7;
		}



		bool is_r() const {
			return (_data & 0x0008) != 0;
		}

		bool is_64() const {
			return (_data & 0x0500) != 0;
		}

		bool operator==(const Register& r) const {
			return r._data == _data;
		}

	private:
		u32 _data;
};


class RegisterOffset {
	public:
		RegisterOffset(Register reg, u32 offset) : _reg(reg), _offset(offset) {
		}

		u32 offset() const {
			return _offset;
		}

		Register reg() {
			return _reg;
		}

	private:
		Register _reg;
		u32 _offset;
};

inline RegisterOffset operator+(Register reg, i32 offset) {
	return RegisterOffset(reg, u32(offset));
}

inline RegisterOffset operator-(Register reg, i32 offset) {
	return RegisterOffset(reg, u32(-offset));
}

namespace regs {
static Register eax  = Register(0);
static Register ecx  = Register(1);
static Register edx  = Register(2);
static Register ebx  = Register(3);

static Register esp  = Register(4);
static Register ebp  = Register(5);
static Register esi  = Register(6);
static Register edi  = Register(7);

static Register r8d  = Register(8);
static Register r9d  = Register(9);
static Register r10d = Register(10);
static Register r11d = Register(11);

static Register r12d = Register(12);
static Register r13d = Register(13);
static Register r14d = Register(14);
static Register r15d = Register(15);

static Register rax  = Register(0, 64);
static Register rcx  = Register(1, 64);
static Register rdx  = Register(2, 64);
static Register rbx  = Register(3, 64);

static Register rsp  = Register(4, 64);
static Register rbp  = Register(5, 64);
static Register rsi  = Register(6, 64);
static Register rdi  = Register(7, 64);

static Register r8   = Register(8, 64);
static Register r9   = Register(9, 64);
static Register r10  = Register(10, 64);
static Register r11  = Register(11, 64);

static Register r12  = Register(12, 64);
static Register r13  = Register(13, 64);
static Register r14  = Register(14, 64);
static Register r15 = Register(15, 64);



static Register arg0 = ecx;
static Register arg1 = edx;
static Register arg2 = r8d;
static Register arg3 = r9d;

}


#endif // REGISTER_H
