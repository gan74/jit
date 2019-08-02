cmake_minimum_required(VERSION 3.6)
project(jit)

set(EXTRA_WARNINGS -Wzero-as-null-pointer-constant -Wsign-promo -Wfloat-conversion -Wshadow)
set(MORE_WARNINGS -Woverloaded-virtual -Wnon-virtual-dtor -Wodr -Wdisabled-optimization -Wlogical-op -Wredundant-decls -Wundef -Wtautological-compare)
set(COMPILE_OPTIONS -pedantic -Wall -Wextra ${EXTRA_WARNINGS} ${MORE_WARNINGS})

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -fomit-frame-pointer")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -Wodr")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)


file(GLOB_RECURSE SOURCE_FILES
		"main.cpp"

		"src/*.h"
		"src/*.cpp"
	)


include_directories(./src/)


add_executable(jit "${SOURCE_FILES}")
target_compile_options(jit PRIVATE ${COMPILE_OPTIONS})



