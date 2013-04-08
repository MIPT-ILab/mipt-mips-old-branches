
/**
 * func_sim.cpp
 * programer-visible memory space accesing via memory address.
 * @author Dmitry Borody (dmitriy.borodiy@gmail.com)>
 * Copyright 2013 uArchSim iLab project
 */


// uArchSim modules
#include <func_memory.h>
#include <func_sim.h>

FuncSim::FuncSim( const char *program_filename)
	: memory( program_filename)
{
	pc = memory->startPC();
}

