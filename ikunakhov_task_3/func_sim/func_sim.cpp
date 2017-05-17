/*
 * func_sim.cpp - instruction parser for mips
 * @author Ivan Kunakhov kunakhov.ivan@gmail.com
 */


#include<func_sim.h>

uint32 fetch() const { 
	return mem->read( PC); }

void updatePC( const FuncInstr& instr) { 
	PC = instr->new_PC; }	

void MIPS::read_src( FuncInstr& instr) {
	instr.v_src1 = rf->read( instr.get_src1_num_index());
	instr.v_src2 = rf->read( instr.get_src2_num_index());}

void MIPS::load( FuncInstr& instr, uint32 bytes) {
	instr.v_dst = mem->read( instr.mem_addr, bytes);}

void MIPS::store( const FuncInstr& instr, bytes) {
	mem->write( instr.mem_addr, instr.v_dst, bytes);}

void MIPS::ld_st( FuncInstr& instr) 
{
    if ( instr.format != FuncInstr::FORMAT_I)
        return;
    switch (instr.ld)
    {
    case true :
        load( instr, instr.byte);
        break;
    case false : 
        store(instr, instr.v_dst, instr.byte);
        break;
    }
}

void MIPS::wb(const FuncInstr& instr) 
{
   	if(instr.v_dst < MAX_REG)
	rf->write( instr.get_dst_num_index(), instr.v_dst);
}

void MIPS::read_src( FuncInstr& instr) 
{
    instr.v_src1 = rf->array[instr.get_src1_num_index()];
    instr.v_src2 = rf->array[instr.get_src2_num_index()];
}



void MIPS::updatePC( const FuncInstr& instr) { 
	PC = instr.new_PC; }
	
void MIPS::run( const string& tr, uint instr_to_run){
	this->PC = startPC;
	FuncMemory mem( tr, 32, 10, 12);
	for (uint i = 0; i < instr_to_run; ++i) 
	{
		uint32 instr_bytes;
		insrt_bytes = fetch();
		FuncInstr instr( instr_bytes, PC);
		read_src( instr);
        instr.execute();
        ld_st( instr);
        wb( instr);
        updatePC( instr);

	}
}	
