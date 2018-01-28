
#include <jit/Assembler.h>

#include <bytecode/eval.h>
#include <bytecode/Assembler.h>

#include <chrono>

#include <iostream>
#include <csignal>


template<typename R, typename... Args>
void dump_asm(Fn<R, Args...> fun, usize count = 1) {
	u8* ptr = reinterpret_cast<u8*>(fun);

	do {
		printf("%02x ", *ptr);
		if(*ptr++ == 0xc3) {
			--count;
		}
	} while(count);
	printf("\n");
}

// https://defuse.ca/online-x86-assembler.htm

void asm_main() {
	using namespace jit;
	Assembler a;

	a.push_stack();
	a.mov(regs::rax, 0);
	auto loop = a.label();
	a.cmp(regs::rax, regs::edx);
	auto f = a.je();

	//a.mov(regs::r10d, regs::rcx+regs::rax*4);
	a.lea(regs::r10, regs::rcx+regs::rax*4);
	a.mov(regs::r10d, regs::r10+0);

	auto inc = a.call();

	a.mov(regs::rcx+regs::rax*4, regs::r10d);
	a.add(regs::rax, 1);
	a.jmp(loop);
	f = a;

	a.pop_stack();
	a.ret();

	inc = a;
	a.push_stack();
	a.add(regs::r10d, 1);
	a.pop_stack();
	a.ret();


	MemoryBlock memory;
	auto fn = a.compile<void, i32*, i32>(memory);

	dump_asm(fn, 2);

	i32 buffer[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	fn(buffer + 2, 5);

	for(i32 i : buffer) {
		printf("%i ", i);
	}
	printf("\nok\n");
}




void bc_main() {
	using namespace bytecode;

	Assembler a;

	a.set(0, 6);

	auto fib = a.label();

	a.function_head(5);
	a.set(1, 1);
	auto end = a.jlt(0, 1);

	a.sub(1, 0, 1);
	a.push_arg(1);
	a.call(1, fib);

	a.set(2, -2);
	a.add(2, 0, 2);
	a.push_arg(2);
	a.call(2, fib);

	a.add(1, 1, 2);
	end = a;
	a.ret(1);

	a.dump();

	auto r = eval(a.data()).integer;

	std::cout << "r = " << r << std::endl;
}


int main() {
	bc_main();

	return 0;
}
