
/**
 * func_sim.h
 * programer-visible memory space accesing via memory address.
 * @author Dmitry Borody (dmitriy.borodiy@gmail.com)>
 * Copyright 2013 uArchSim iLab project
 */

// protection from multi-include
#ifndef FUNC_SIM_H
#define FUNC_SIM_H

class FuncSim
{
public:
	FuncSim( const char *filename);

private:
	FuncMemory memory;
	uint64 pc;
};

#endif 


