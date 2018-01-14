
#include <Assembler.h>

#include <iostream>

int main() {
	Assembler a;
	a.push_stack();

	a.mov(regs::eax, regs::arg1);
    a.add(regs::eax, 1);

	a.pop_stack();
	a.ret();

	a.dump();

	auto fn = a.compile<i32, i32, i32>();

	auto r = fn(1, 7);

	std::cout << "result = " << r << std::endl;


	return 0;
}
