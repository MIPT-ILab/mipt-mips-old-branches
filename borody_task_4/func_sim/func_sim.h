
/**
 * func_sim.h
 * programer-visible memory space accesing via memory address.
 * @author Dmitry Borody (dmitriy.borodiy@gmail.com)>
 * Copyright 2013 uArchSim iLab project
 */

// protection from multi-include
#ifndef FUNC_SIM_H
#define FUNC_SIM_H

// Generic C++
#include <string>

// uArchSim modules
#include <func_memory.h>
#include <func_instr.h>
#include <reg_file.h>

class FuncSim
{
public:
	FuncSim( const char *filename);

	void doSimulationStep();
	bool isTerminalConditionReached() const;
	std::string dumpCurrentInstruction() const;
	uint64 getPC() const;
	uint64 getRegisterValue( size_t register_id) const;

private:
	FuncMemory memory;
	RegFile regfile;
	uint64 pc;
	std::string cur_instr_dump;
	bool is_terminal_condition_reached;
};

#endif 


