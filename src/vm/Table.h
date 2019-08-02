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
#ifndef JIT_TABLE_H
#define JIT_TABLE_H

#include "Value.h"

#include <vector>
#include <unordered_map>

namespace jit {

class Table {
	struct value_hash {
		using argument_type = Value;
		using result_type = u64;
		result_type operator()(const argument_type& v) const noexcept;
	};

	public:
		usize size() const;

		void set(const Constant& cst, const Value& value);
		void set(const Value& key, const Value& value);


		Value get(const Constant& cst);
		Value get(const Value& key);

		auto begin() {
			return _storage.begin();
		}

		auto end() {
			return _storage.end();
		}

		auto find(const Value& key) {
			return _storage.find(key);
		}

	private:
		std::unordered_map<Value, Value, value_hash> _storage;
};


}

#endif // JIT_TABLE_H
