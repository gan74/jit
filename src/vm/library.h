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
#ifndef JIT_LIBRARY_H
#define JIT_LIBRARY_H

#include "Value.h"
#include "Table.h"

namespace jit {
namespace lib {


Table default_env();

void print(const Value& v);
u32 print(MutableSpan<Value>, Span<Value> in);

u32 pairs(MutableSpan<Value> out, Span<Value> in);
u32 ipairs(MutableSpan<Value> out, Span<Value> in);

u32 math_sqrt(MutableSpan<Value> out, Span<Value> in);

u32 io_read(MutableSpan<Value> out, Span<Value> in);

u32 table_insert(MutableSpan<Value> out, Span<Value> in);

}
}

#endif // JIT_LIBRARY_H
