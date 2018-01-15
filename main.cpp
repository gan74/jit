
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

int main() {

	Assembler a;
	a.push_stack();


	a.cmp(regs::eax, regs::eax);
	auto f = a.je();
	auto loop = a.label();
	a.call(counter);
	a.mov(regs::ecx, 4);
	a.cmp(regs::eax, regs::ecx);
	a.jne(loop);
	f = a;


	a.pop_stack();
	a.ret();



	auto fn = a.compile<void>();

	dump(fn);


	fn();
	printf("ok\n");




	return 0;
}
