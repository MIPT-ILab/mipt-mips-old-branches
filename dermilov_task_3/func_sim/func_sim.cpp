#include "func_sim.h"
#include <cstring>

MIPS::MIPS()
{
    rf = new RF;
    memset( rf, 0, sizeof( uint32) * MAX_REG);
}

MIPS::~MIPS()
{
    delete rf;
    delete mem;
}

void MIPS::run( const string& tr, uint32 instr_to_run)
{
    char * cstr = new char [tr.length()+1];
    std::strcpy (cstr, tr.c_str());
    cstr[ tr.length()] = '\0';

    mem = new FuncMemory( cstr, 32, 10, 12);

    this->PC = ( uint32) mem->startPC();
    for (uint32 i = 0; i < instr_to_run; ++i) 
    {
        uint32 instr_bytes;
        //+ Fetch
        instr_bytes = fetch();
        // Decode
        FuncInstr instr( instr_bytes, PC);
        // Read sources
        read_src( instr);
        // Execute
        instr.execute();
        // Memory access
        ld_st( instr);
        // Update PC, Writeback and Dump
        write_back( instr);
    }
}

void MIPS::write_back( FuncInstr& instruction)
{
    updatePC( instruction);
    switch( instruction.format)
    {
        case FORMAT_R:
            rf->write(  ( RegNum) instruction .instr .asR .rd, instruction.v_dst);
            instruction.dump_R();
            break;
        case FORMAT_I:
            rf->write( ( RegNum) instruction .instr .asI .rt, instruction.v_src1);
            instruction.dump_I();
            break;
        case FORMAT_J:
            instruction.dump_J();
            break;
        case FORMAT_UNKNOWN:
            instruction.dump_Unknown();
    }
    cout << instruction << endl;
}

void MIPS::read_src( FuncInstr& instr) 
{
    if  ( ( instr.format == FORMAT_R) | ( instr.format == FORMAT_I))
    {
        instr.v_src1 = rf->read( ( RegNum) instr.get_src1_num_index());
        instr.v_src2 = rf->read( ( RegNum) instr.get_src2_num_index());
    }
    // ...
}

void MIPS::load( FuncInstr& instr) 
{
    instr.v_dst = mem->read( instr.mem_addr);                //simulate the choice of 
    instr.fit_mem_size();                                    //necessary load instruction
}

void MIPS::store( /*const*/ FuncInstr& instr) 
{
    instr.fit_mem_size();                                   //simulate the choice of 
    mem->write( instr.mem_addr, instr.v_dst);               //necessary store instruction
} 

void MIPS::ld_st( FuncInstr& instr) 
{
    if  ( instr.check())
    {
        if  ( ( instr.format == FORMAT_I) && ( instr.operation == OUT_I_LOAD) 
           && ( instr.mem_addr))
            load( instr);
        if  ( ( instr.format == FORMAT_I) && ( instr.operation == OUT_I_STORE) 
           && ( instr.mem_addr))
            store( instr);
    }
}

uint32 RF::read( RegNum index) const
{
	assert( check( index));
	return array[ index]; 
}

bool RF::check( RegNum index) const
{
	return ( index >= ZERO) && ( index < MAX_REG);
}

void RF::write( RegNum index, uint32 data)
{
	assert( check( index));

	if  ( index != ZERO)
		array[ index] = data;
}

void RF::reset( RegNum index)
{
	assert( check( index));
	array[ index] = 0;
}