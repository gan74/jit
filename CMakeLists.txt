cmake_minimum_required(VERSION 3.6)
project(jit)

include_directories(${jit_SOURCE_DIR})


set(ULTIMATE_WARNINGS "-Wdisabled-optimization -Wlogical-op -Wwrite-strings -Wmissing-include-dirs -Wredundant-decls -Wsign-promo -Wswitch-default -Wundef")
set(EXTRA_WARNINGS "-Woverloaded-virtual -Wcast-qual -Wcast-align -Wunreachable-code -Wold-style-cast -Wuseless-cast -Wnon-virtual-dtor -Wodr")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

add_definitions("${EXTRA_WARNINGS}")
add_definitions("${ULTIMATE_WARNINGS}")
add_definitions("-std=c++17 -pedantic -Wall -Wextra")


file(GLOB_RECURSE SOURCE_FILES
		"main.cpp"

		"src/*.h"
		"src/*.cpp"
	)


include_directories(./src/)


add_executable(jit "${SOURCE_FILES}")



