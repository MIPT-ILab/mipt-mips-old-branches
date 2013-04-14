
/**
 * func_sim.cpp
 * programer-visible memory space accesing via memory address.
 * @author Dmitry Borody (dmitriy.borodiy@gmail.com)>
 * Copyright 2013 uArchSim iLab project
 */


// uArchSim modules
#include <func_memory.h>
#include <func_sim.h>
#include <func_instr.h>

FuncSim::FuncSim( const char *program_filename)
	: memory( program_filename), regfile( 128), cur_instr_dump( "")
{
	pc = memory.startPC();
	is_terminal_condition_reached = false;
}

void FuncSim::doSimulationStep()
{
	if ( is_terminal_condition_reached)
		return;
	FuncInstr cur_instr = FuncInstr( memory.read( pc, 4));
	cur_instr_dump = cur_instr.dumpWithRegisterValues( regfile);
	pc = cur_instr.execute( pc, memory, regfile);
	is_terminal_condition_reached = cur_instr.isTerminalInstruction();
}

bool FuncSim::isTerminalConditionReached() const 
{
	return is_terminal_condition_reached;
}

std::string FuncSim::dumpCurrentInstruction() const
{
	return cur_instr_dump;
}

uint64 FuncSim::getPC() const
{
    return pc;
}