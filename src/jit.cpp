/*#include <vector>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <windows.h>


using u8 = std::uint8_t;
using u16 = std::uint16_t;

using i32 = std::int32_t;

template<typename R, typename... Args>
using fn_ptr = __cdecl R(*)(Args...);

void fatal() {
    std::exit(-1);
}

union i32_to_u8 {
	i32 i;
	u8 bytes[sizeof(i32)];
};

struct Assembler {
	std::vector<u8> bytes;

    template<typename... Args>
    void push(u8 b, Args... args) {
		bytes.push_back(b);
        if constexpr(sizeof...(args)) {
            push(args...);
        }
	}

	void push_i32(i32 value) {
		i32_to_u8 i;
		i.i = value;
		for(u8 u : i.bytes) {
			push(u);
		}
	}


	void push_stack() {
		// push   %rbp
		push(0x55);
		// mov    %rsp,%rbp
		push(0x48, 0x89, 0xe5);
	}

	void pop_stack() {
		// pop    %rbp
		push(0x5d);
	}

	void ret() {
		// retq
		push(0xc3);
	}

	void arg_to_eax(u8 arg_index = 0) {
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

#include <iostream>

__cdecl i32 test(i32 a, i32 b, i32 c, i32 d, i32 e, i32 f, i32 g, i32 h, i32 i) {
   return b;
}

int main() {
	Assembler a;
	a.push_stack();

    a.arg_to_eax(7);
    a.add_eax(1);
    //a.mov_eax_edi();

	a.pop_stack();
	a.ret();

	auto fn = a.as_function<i32, i32, i32, i32, i32, i32, i32, i32, i32>();
	int i = fn(1, 2, 3, 4, 5, 6, 7, 8);

	std::cout << i << std::endl;



	return 0;
}
static_assert(sizeof(void*) == 8);*/
