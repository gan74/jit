
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

int main() {

	Assembler a;
	a.push_stack();

	a.call(a_function);
	a.call(a_function);
	a.call(a_function);
	a.call(a_function);
	a.call(a_function);

	a.pop_stack();
	a.ret();



	auto fn = a.compile<void, i32*>();

	dump(fn);


	fn(nullptr);
	printf("ok\n");




	return 0;
}
