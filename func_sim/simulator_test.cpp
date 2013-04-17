// generic C
#include <cassert>
#include <cstdlib>

// generic C++
#include <string>

// Google Test library
#include <gtest/gtest.h>

// uArchSim modules
#include <elf_parser.h>
#include <func_sim.h>

using namespace std;

TEST( FuncSim_tests, load_program)
{   
	const char *test_program = "./tests/add.out";
	FuncSim sim( test_program);
	SUCCEED();
}

TEST( FuncSim_tests, dump_instruction_add)
{
    const char *test_program = "./tests/add.out";
    FuncSim sim( test_program);
    string expected_dump = "add   $t0(0), $t1(0), $t2(0)";
    sim.doSimulationStep();
    string dump = sim.dumpCurrentInstruction();
    EXPECT_EQ( expected_dump, dump);
}

TEST( FuncSim_tests, dump_two_add_instructions)
{
    const char *test_program = "./tests/add.out";
    FuncSim sim( test_program);
    string expected_dump = "add   $t0(0), $t1(0), $t2(0)";
    sim.doSimulationStep();
    string dump = sim.dumpCurrentInstruction();
    EXPECT_EQ( expected_dump, dump);   
    sim.doSimulationStep();
    expected_dump = "add   $v0(0), $t3(0), $t1(0)";
    dump = sim.dumpCurrentInstruction();
    EXPECT_EQ( expected_dump, dump);
}

TEST( FuncSim_tests, check_termination_condition)
{
    const char *test_program = "./tests/termination.out";
    FuncSim sim( test_program);
    string expected_dump = "add   $v0(0), $t1(0), $t2(0)";
    sim.doSimulationStep();
    string dump = sim.dumpCurrentInstruction();
    EXPECT_EQ( expected_dump, dump);   
    EXPECT_TRUE( sim.isTerminalConditionReached());
}

TEST( FuncSim_tests, instruction_lui)
{
    const char *test_program = "./tests/lui.out";
    FuncSim sim( test_program);
    string expected_dump = "lui   $t0(0), 0xDEAD";
    sim.doSimulationStep();
    string dump = sim.dumpCurrentInstruction();
    EXPECT_EQ( expected_dump, dump);   
    sim.doSimulationStep();
    expected_dump = "lui   $t0(3735879680), 0xBEEF";
    dump = sim.dumpCurrentInstruction();
    EXPECT_EQ( expected_dump, dump);   
    sim.doSimulationStep();
    expected_dump = "lui   $v0(0), 0xBEEF";
    dump = sim.dumpCurrentInstruction();
    EXPECT_EQ( expected_dump, dump);   
    EXPECT_TRUE( sim.isTerminalConditionReached());
}

TEST( FuncSim_tests, instructions_add_sub)
{
    const char *test_program = "./tests/arith.out";
    FuncSim sim( test_program);
    sim.doSimulationStep();
    string expected_dump = "addiu $t0(0), $t0(0), 57005";
    string dump = sim.dumpCurrentInstruction();
    EXPECT_EQ( expected_dump, dump);   
    sim.doSimulationStep();
    expected_dump = "addiu $t1(0), $t1(0), 1";
    dump = sim.dumpCurrentInstruction();
    EXPECT_EQ( expected_dump, dump);   
    sim.doSimulationStep();
    expected_dump = "add   $t3(0), $t0(57005), $t1(1)";
    dump = sim.dumpCurrentInstruction();
    EXPECT_EQ( expected_dump, dump);   
    sim.doSimulationStep();
    expected_dump = "sub   $t3(57006), $t1(1), $t3(57006)";
    dump = sim.dumpCurrentInstruction();
    EXPECT_EQ( expected_dump, dump);   
    sim.doSimulationStep();
    expected_dump = "addu  $v0(0), $t3(57005), $zero(0)";
    dump = sim.dumpCurrentInstruction();
    EXPECT_EQ( expected_dump, dump);   
    EXPECT_TRUE( sim.isTerminalConditionReached());
}

TEST( FuncSim_tests, instructions_shifts)
{
    const char *test_program = "./tests/shifts.out";
    FuncSim sim( test_program);
    sim.doSimulationStep();
    string expected_dump = "addi  $t0(0), $t0(0), 4096";
    string dump = sim.dumpCurrentInstruction();
    EXPECT_EQ( expected_dump, dump);   
    sim.doSimulationStep();
    expected_dump = "sll   $t1(0), $t0(4096), 2";
    dump = sim.dumpCurrentInstruction();
    EXPECT_EQ( expected_dump, dump);   
    sim.doSimulationStep();
    expected_dump = "srl   $t1(16384), $t0(4096), 2";
    dump = sim.dumpCurrentInstruction();
    EXPECT_EQ( expected_dump, dump);   
    sim.doSimulationStep();
    expected_dump = "addu  $v0(0), $t1(1024), $zero(0)";
    dump = sim.dumpCurrentInstruction();
    EXPECT_EQ( expected_dump, dump);   
    EXPECT_TRUE( sim.isTerminalConditionReached());
}

TEST( FuncSim_tests, instructions_store_load)
{
    const char *test_program = "./tests/store_load.out";
    FuncSim sim( test_program);
    sim.doSimulationStep();
    sim.doSimulationStep();
    sim.doSimulationStep();
    sim.doSimulationStep();
    sim.doSimulationStep();
    string dump = sim.dumpCurrentInstruction();
    string expected_dump = "add   $t2(0), $t2(0), $t0(11)";
    EXPECT_EQ( expected_dump, dump);
    sim.doSimulationStep();
    sim.doSimulationStep();
    sim.doSimulationStep();
    sim.doSimulationStep();
    dump = sim.dumpCurrentInstruction();
    expected_dump = "add   $t3(0), $t3(0), $t0(42)";
    EXPECT_EQ( expected_dump, dump);
    sim.doSimulationStep();
    EXPECT_TRUE( sim.isTerminalConditionReached());
}

TEST( FuncSim_tests, instructions_jr)
{
    const char *test_program = "./tests/jr.out";
    FuncSim sim( test_program);
    sim.doSimulationStep();
    sim.doSimulationStep();
    EXPECT_EQ( 0xDEAD, sim.getPC());
}

TEST( FuncSim_tests, instructions_beq)
{
    const char *test_program = "./tests/beq.out";
    FuncSim sim( test_program);
    uint64 pc = sim.getPC();
    sim.doSimulationStep();
    sim.doSimulationStep();
    sim.doSimulationStep();
    sim.doSimulationStep();
    sim.doSimulationStep();
    sim.doSimulationStep();
    EXPECT_EQ( pc, sim.getPC());
}

TEST( FuncSim_tests, instructions_bne)
{
    const char *test_program = "./tests/bne.out";
    FuncSim sim( test_program);
    uint64 pc = sim.getPC();
    sim.doSimulationStep();
    sim.doSimulationStep();
    sim.doSimulationStep();
    sim.doSimulationStep();
    sim.doSimulationStep();
    sim.doSimulationStep();
    EXPECT_EQ( pc, sim.getPC());
}