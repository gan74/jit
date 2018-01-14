#include <vector>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <windows.h>


using u8 = std::uint8_t;
using i32 = std::int32_t;

template<typename R, typename... Args>
using fn_ptr = R(*)(Args...);

union i32_to_u8 {
	i32 i;
	u8 bytes[sizeof(i32)];
};

struct Assembler {
	std::vector<u8> bytes;
	
	void push(u8 b) {
		bytes.push_back(b);
	}
	
	void push_i32(i32 value) {
		i32_to_u8 i;
		i.i = value;
		for(u8 u : i.bytes) {
			push(u);
		}
	}
	
	
	void push_stack() {
		// push rbp
		push(0x55);
		// mov rbp,rsp
		push(0x48);
		push(0x89);
		push(0xe5);
	}
	
	void pop_stack() {
		// pop rbp
		push(0x5d);	
	}
	
	void ret() {
		// ret
		push(0xc3);
	}
	
	void first_arg_to_edi() {
		// mov DWORD PTR [rbp-0x4],edi
		push(0x89);
		push(0x7d);
		push(0xfc);
		// mov eax,DWORD PTR [rbp-0x4]
		push(0x8b);
		push(0x7d); // this is eax
		push(0xfc);
	}
	
	
	void mov_eax(i32 value) {
		// mov eax value
		push(0xb8);
		push_i32(value);
	}
	
	void add_eax(i32 value) {
		// add eax,value
		push(0x05);
		push_i32(value);
	}
	
	void mov_eax_edi() {
		push(0x89);
		push(0xf8);
	}
	template<typename R, typename... Args>
	fn_ptr<R, Args...> as_function() const {
		static constexpr size_t page_size = 4096;
		void* buffer = VirtualAlloc(nullptr, page_size, MEM_COMMIT, PAGE_READWRITE);
		std::memcpy(buffer, bytes.data(), bytes.size());
		DWORD res = 0;
		VirtualProtect(buffer, bytes.size(), PAGE_EXECUTE_READ, &res);
		return reinterpret_cast<fn_ptr<R, Args...>>(buffer);
	}
};

static_assert(sizeof(void*) == 8, "not 64bits");


#include <iostream>

int main() {
	Assembler a;
	a.push_stack();
	
	a.mov_eax_edi();
	
	a.pop_stack();
	a.ret();
	
	auto fn = a.as_function<i32, i32>();
	int i = fn(4);
	
	std::cout << i << std::endl;
	
	
	return 0;
}