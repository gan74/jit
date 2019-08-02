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

#include "Table.h"
#include "library.h"

#include <algorithm>

namespace jit {


Table::value_hash::result_type Table::value_hash::operator()(const argument_type& v) const noexcept {
	if(v.type == ValueType::String) {
		std::string_view str = v.string();
		return std::hash<std::string_view>()(str);
	}
	return v.integer;
}

usize Table::size() const {
	return _storage.size();
}

void Table::set(const Constant& cst, const Value& value) {
	set(Value(cst), value);
}

void Table::set(const Value& key, const Value& value) {
	auto it = find(key);
	if(value.type == ValueType::None) {
		if(it != end()) {
			_storage.erase(it);
		}
		return;
	}

	if(it == end()) {
		_storage[key] = value;
	} else {
		it->second = value;
	}
}

Value Table::get(const Constant& cst) {
	return get(Value(cst));
}

Value Table::get(const Value& key) {
	if(auto it = find(key); it != end()) {
		return it->second;
	}

	return Value();
}

}
