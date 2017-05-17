/*******************************************************

    Implementation for a functional simulator

    @file: func_sim.cpp
    @date: March 7, 2015
    @author: Kirill Korolev <kirill.korolef@gmail.com>
    @vertion: 1.0 (March 7, 2015) 

*******************************************************/

#include <func_sim.h>

uint32 RF::read(RegNum index) const 
{
	if (index < MAX_REG) return array[index];
	return 0;
}

void MIPS::read_src(FuncInstr &instr) 
{
	instr.v_src1 = rf->read(instr.get_src1_num_index());
	instr.v_src2 = rf->read(instr.get_src2_num_index());
}

void MIPS::ld_st(FuncInstr &instr) 
{
        if (!instr.mem_addr) return;
        if (instr.is_load) load(instr);
        else              store(instr);
}

void MIPS::run(const char *tr, uint instr_to_run)
{
	mem = new FuncMemory(tr);
	PC = mem->startPC();

	for (int i = 0; i < instr_to_run; ++i) {
		uint32 instr_bytes = fetch();     //FETCH
		FuncInstr instr(instr_bytes, PC); //DECODE
		read_src(instr);                  //READ SOURCE
		instr.execute();                  //EXECUTE
		ld_st(instr);                     //MEMORY ACCESS
		wb(instr);                        //WRITEBACK
		updatePC(instr);                  //UPDATE PC
		std::cout << instr << std::endl;   //DUMP
	}

	delete mem;
}