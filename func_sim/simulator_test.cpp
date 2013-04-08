// generic C
#include <cassert>
#include <cstdlib>

// Google Test library
#include <gtest/gtest.h>

// uArchSim modules
#include <elf_parser.h>
#include <func_sim.h>

TEST( Simulator_tests, FuncSim_load_program)
{   
	const char *test_program = "tests/empty_program.elf";
	FuncSim sim( test_program);
	SUCCEED();
}
