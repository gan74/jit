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
#ifndef JIT_ASSEMBLER_H
#define JIT_ASSEMBLER_H

#include <vector>

#include "Register.h"
#include "MemoryBlock.h"

namespace jit {

class Assembler {

	public:
		class Label {
			friend class Assembler;

			i32 operator-(const Label& o) const {
				return i32(_index - o._index);
			}

			Label(u32 i) : _index(i) {
			}

			u32 _index;
		};

		class ForwardLabel {
			friend class Assembler;

			ForwardLabel(Label l) : _index(l._index) {
			}

			u32 _index;

			public:
				void operator=(Assembler& a) const {
					a.forward_label(_index);
				}

				operator Label() const {
					return Label(_index);
				}
		};

		Assembler();

		template<typename R, typename... Args>
		Fn<R, Args...> compile(MemoryBlock& block) const {
			if(block.size() < _bytes.size()) {
				return nullptr;
			}
			return reinterpret_cast<Fn<R, Args...>>(copy_compile(block.data()));
		}


		void push_stack();
		void pop_stack();
		void ret();

		void nop();

		void set_zero(Register dst);


		void mov(Register dst, Register src);
		void mov(Register dst, i32 value);

		void mov(RegisterOffset dst, Register src);
		void mov(RegisterIndexOffset dst, Register src);
		void mov(RegisterIndexOffsetRegister dst, Register src);

		void mov(Register dst, RegisterOffset src);
		void mov(Register dst, RegisterIndexOffset src);
		void mov(Register dst, RegisterIndexOffsetRegister src);

		void lea(Register dst, RegisterOffset src);
		void lea(Register dst, RegisterIndexOffset src);
		void lea(Register dst, RegisterIndexOffsetRegister src);


		void add(Register dst, Register src);
		void add(Register dst, i32 value);

		void sub(Register dst, Register src);
		void sub(Register dst, i32 value);


		//void imul(Register b);
		void imul(Register dst, i32 value);
		void imul(Register dst, Register src);
		void imul(Register dst, Register src, i32 value);

		void inc(Register dst);

		void xor_(Register dst, Register src);

		void cmp(Register a, Register b);

		void je(Label to);
		ForwardLabel je();

		void jne(Label to);
		ForwardLabel jne();

		void jmp(Label to);
		ForwardLabel jmp();


		void call(Register fn);
		void call(Label fn);
		ForwardLabel call();
		void call(void* fn_ptr);

		template<typename R, typename... Args>
		void call(Fn<R, Args...> fn_ptr) {
			call(reinterpret_cast<void*>(fn_ptr));
		}


		Label label() const;

	private:
		std::vector<u8> _bytes;
		std::vector<std::pair<Label, u8*>> _calls;

		void* copy_compile(void* buffer) const;

		void forward_label(u32 label);

		void r_prefix(Register dst);
		void r_prefix(Register a, Register b);
		void r_prefix_with_index(Register a, Register b);


		void bin_op_instr(u8 opcode, Register dst, Register src, u8 base = 0xc0);
		void bin_op_instr(u8 opcode, Register dst, i32 value, u8 base = 0xc0);

		void addr_instr(u8 opcode, Register dst, RegisterOffset src);
		void addr_instr(u8 opcode, Register dst, RegisterIndexOffset src);
		void addr_instr(u8 opcode, Register dst, RegisterIndexOffsetRegister src);





		template<typename... Args>
		void push(u8 b, Args... args) {
			_bytes.push_back(b);
			if constexpr(sizeof...(args)) {
				push(args...);
			}
		}

		void push_i32(i32 value) {
			union i32_to_u8 {
				i32 i;
				u8 bytes[sizeof(i32)];
			};
			i32_to_u8 i;
			i.i = value;
			for(u8 u : i.bytes) {
				push(u);
			}
		}
};
}

#endif // JIT_ASSEMBLER_H
