
#include <Assembler.h>

#include <iostream>
#include <csignal>

void handler(int a) {
    std::cout << "Signal " << a << " here!" << std::endl;
}

template<typename R, typename... Args>
void dump(Fn<R, Args...> fun) {
	u8* ptr = reinterpret_cast<u8*>(fun);

	do {
		printf("%02x ", *ptr);
	} while(*ptr++ != 0xc3);
	printf("\n");
}


__cdecl void a_function() {
	printf("function called\n");
}

__cdecl i32 counter() {
	static i32 c = 0;
	std::cout << "c = " << c << std::endl;
	return c++;
}


// https://defuse.ca/online-x86-assembler.htm

int main() {

	Assembler a;
	a.push_stack();


	// first arg in rcx, second in edx
	// counter in rax
	a.mov(regs::rax, 0);
	auto loop = a.label();
	a.cmp(regs::rax, regs::edx);
	auto f = a.je();
	//a.mov(regs::r10d, regs::rcx+regs::rax*4);
	a.lea(regs::r10, regs::rcx+regs::rax*4);
	a.mov(regs::r10d, regs::r10+0);

	a.add(regs::r10d, 1);
	a.mov(regs::rcx+regs::rax*4, regs::r10d);
	a.add(regs::rax, 1);
	a.jmp(loop);
	f = a;

	a.pop_stack();
	a.ret();



	auto fn = a.compile<void, i32*, i32>();

	dump(fn);

	i32 buffer[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	fn(buffer + 2, 5);

	for(i32 i : buffer) {
		printf("%i ", i);
	}
	printf("\nok\n");




	return 0;
}
