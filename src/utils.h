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
#ifndef UTILS_H
#define UTILS_H

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <array>

#include <cassert>

namespace jit {

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using usize = std::size_t;

bool is_8_bits(i32 x);

void fatal(const char* msg);

template<typename R, typename... Args>
using Fn = __cdecl R (*)(Args...);

static_assert(sizeof(void*) == 8);

template<typename... Args>
void unused(Args...) {}


template<typename F>
class ScopeExit {
	F _f;
	public:
		ScopeExit(F&& f) : _f(f) {}
		~ScopeExit() { _f(); }
};

#define SCOPE_EXIT_HELPER(a, b) a ## b
#define SCOPE_EXIT(expr) auto SCOPE_EXIT_HELPER(scope_exit_, __LINE__) = ScopeExit([&] { expr; });


template<typename T>
class MutableSpan {

	public:
		using value_type = T;
		using iterator = T*;
		using const_iterator = const T*;

		constexpr MutableSpan() = default;

		constexpr MutableSpan(const MutableSpan&) = default;
		constexpr MutableSpan& operator=(const MutableSpan&) = default;

		MutableSpan(std::nullptr_t) {
		}

		MutableSpan(T& t) : _data(&t), _size(1) {
		}

		MutableSpan(T* data, usize size) : _data(data), _size(size) {
		}

		template<usize N>
		MutableSpan(T(&arr)[N]) : _data(arr), _size(N) {
		}

		template<usize N>
		MutableSpan(std::array<T, N>& arr) : _data(arr.data()), _size(N) {
		}

		MutableSpan(std::initializer_list<T> l) : _data(l.begin()), _size(l.size()) {
		}

		usize size() const {
			return _size;
		}

		bool is_empty() const {
			return !_size;
		}

		T* data() {
			return _data;
		}

		const T* data() const {
			return _data;
		}

		iterator begin() {
			return _data;
		}

		iterator end() {
			return _data + _size;
		}

		const_iterator begin() const {
			return _data;
		}

		const_iterator end() const {
			return _data + _size;
		}

		const_iterator cbegin() const {
			return _data;
		}

		const_iterator cend() const {
			return _data + _size;
		}

		T& operator[](usize i) const {
			assert(i < _size);
			return _data[i];
		}

	private:
		T* _data = nullptr;
		usize _size = 0;

};

template<typename T>
using Span = MutableSpan<const T>;

template<typename T>
using ArrayView = Span<T>;

}



#endif // UTILS_H
