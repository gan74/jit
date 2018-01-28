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
#ifndef BYTECODE_ASSEMBLER_H
#define BYTECODE_ASSEMBLER_H

#include <vector>

#include "Bytecode.h"

namespace bytecode {

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

		Assembler() = default;

		void dump() const;

		void function_head(Register regs);
		void ret(Register src);
		void push_arg(Register src);

		void call(Register dst, Label fn);
		ForwardLabel call(Register dst);


		void add(Register dst, Register a, Register b);
		void sub(Register dst, Register a, Register b);
		void mul(Register dst, Register a, Register b);
		void div(Register dst, Register a, Register b);

		void cpy(Register dst, Register src);
		void set(Register dst, i32 value);


		void jlt(Register a, Register b, Label to);
		ForwardLabel jlt(Register a, Register b);


		Label label() const;

		const Bytecode* data() const {
			return reinterpret_cast<const Bytecode*>(_bytes.data());
		}

	private:
		std::vector<u8> _bytes;

		void forward_label(u32 label);

		template<typename T, typename... Args>
		void push(T t, Args... args) {
			push_u8(t);
			if constexpr(sizeof...(args)) {
				push(args...);
			}
		}

		void push_u8(u8 b) {
			_bytes.push_back(b);
		}

		void push_u8(Bytecode b) {
			_bytes.push_back(u8(b));
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

#endif // BYTECODE_ASSEMBLER_H

