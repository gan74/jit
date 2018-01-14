
#include <Assembler.h>

#include <iostream>

int main() {
	Assembler a;
	a.push_stack();

	a.mov(regs::ebx, regs::r13d - 1);
	a.mov(regs::r13d + 9, regs::ebx);

	a.pop_stack();
	a.ret();

	a.dump();

	//auto fn = a.compile<i32, i32, i32>();
	//auto r = fn(2, 7);
	//std::cout << "result = " << r << std::endl;


	return 0;
}
