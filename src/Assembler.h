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
#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <vector>

#include "utils.h"

class Assembler
{
	public:
		Assembler();

		template<typename R, typename... Args>
		Fn<R, Args...> compile() const {
			return reinterpret_cast<Fn<R, Args...>>(alloc_compile());
		}


		void push_stack();
		void pop_stack();
		void ret();

		void mov_eax(i32 value);
		void add_eax(i32 value);

		void arg_to_eax(u8 arg_index);

	private:
		std::vector<u8> _bytes;


		void* alloc_compile() const;





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

#endif // ASSEMBLER_H
