

#include <fstream>
#include <memory>

#include "vm/VM.h"
#include "vm/library.h"
#include "vm/exceptions.h"

using namespace jit;

std::vector<u8> read_file(const char* filename) {
	std::ifstream file(filename, std::ios_base::in | std::ios_base::binary | std::ios_base::ate);
	if(!file.is_open()) {
		fatal("Bytecode file not found.");
	}
	usize size = file.tellg();
	file.seekg(0, file.beg);

	std::vector<u8> data(size, 0x00);
	file.read(reinterpret_cast<char*>(data.data()), size);
	return data;
}


void lua_main() {
	auto luac = read_file("../../luac.out");

	Program program = Program::from_luac(ArrayView<u8>(luac.data(), luac.size()));

	Table env = lib::default_env();

	VM vm(&env);

	Value ret;
	try {
		vm.eval(program, &ret);
	} catch(ExecutionException& e) {
		std::printf("ERROR: %s at instruction %s\n", e.what(), op_name(OpCode(e.instruction->opcode)));
	}
}

int main() {
	lua_main();

	return 0;
}
