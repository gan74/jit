
#include <Assembler.h>

#include <iostream>

int main() {
	Assembler a;
	a.push_stack();

    a.arg_to_eax(1);
    a.add_eax(1);
    //a.mov_eax_edi();

	a.pop_stack();
	a.ret();

	auto fn = a.compile<i32, i32, i32>();

	auto r = fn(1, 7);

	std::cout << "result = " << r << std::endl;

	return 0;
}
