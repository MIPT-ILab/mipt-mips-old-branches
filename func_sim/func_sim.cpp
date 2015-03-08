/**
 * func_sim.cpp - simulator of MIPS CPU
 * @author Semyon Abramov <semyon.abramov.mipt@gmail.com>
 * 2014 - 2015 iLab project: MIPT - MIPS
 */

//Generic C++
#include<iostream>

//Generic uArchSim modules
#include"func_sim.h"

/** 
  * MIPS class methods 
  */

MIPS::MIPS()
{
	this->rf = new RF;
}

MIPS::~MIPS()
{
	delete rf;
}

/* fetch instruction */

uint32 MIPS::fetch() const
{
	return mem->read( PC);
}

/* read sources */

void MIPS::read_src( FuncInstr& instr) 
{
	instr.v_src1 = rf->read( static_cast< RegNum>( instr.get_src1_num_index()));
	instr.v_src2 = rf->read( static_cast< RegNum>( instr.get_src2_num_index()));
	instr.C = instr.getImm();
	instr.CJump = instr.getJImm();
	instr.Sh = instr.getShamt();
}

/* load from Memory to Registers */

void MIPS::load(FuncInstr& instr) 
{
    if ( instr.pointer == &FuncInstr::lb)
	{
		rf->write( static_cast< RegNum>( instr.get_dst_num_index()), int8( mem->read( instr.mem_addr, 1)));
		instr.v_dst = rf->read( static_cast< RegNum>( instr.get_dst_num_index()));
		return;
	}
	
	if ( instr.pointer == &FuncInstr::lbu)
	{
		rf->write( static_cast< RegNum>( instr.get_dst_num_index()), uint8( mem->read( instr.mem_addr, 1)));
		instr.v_dst = rf->read( static_cast< RegNum>( instr.get_dst_num_index()));
		return;
	}
	
	if ( instr.pointer == &FuncInstr::lw)
	{
		rf->write( static_cast< RegNum>( instr.get_dst_num_index()), mem->read( instr.mem_addr, 4));
		instr.v_dst = rf->read( static_cast< RegNum>( instr.get_dst_num_index()));
		return;
	}
	
	if ( instr.pointer == &FuncInstr::lh) 
	{
		rf->write( static_cast< RegNum>( instr.get_dst_num_index()), int16( mem->read( instr.mem_addr, 2)));
		instr.v_dst = rf->read( static_cast< RegNum>( instr.get_dst_num_index()));
		return;
	}
	
	if ( instr.pointer == &FuncInstr::lhu)
	{
		rf->write( static_cast< RegNum>( instr.get_dst_num_index()), uint16( mem->read( instr.mem_addr, 2)));
		instr.v_dst = rf->read( static_cast< RegNum>( instr.get_dst_num_index()));
		return;
	}
	return;
}

/* store to Memory */

void MIPS::store( FuncInstr& instr) 
{
	instr.v_dst  = rf->read( static_cast< RegNum>( instr.get_dst_num_index()));
	
	if ( instr.pointer == &FuncInstr::sb)
	{	
		instr.v_dst = instr.v_dst & 0xff;
		mem->write( instr.v_dst, instr.mem_addr, 1);
	}
	
	if ( instr.pointer == &FuncInstr::sh)
	{
		instr.v_dst = instr.v_dst & 0xffff;
		mem->write( instr.v_dst, instr.mem_addr, 2);
	}
	
	if ( instr.pointer == &FuncInstr::sw)
	{
		mem->write( instr.v_dst, instr.mem_addr, 4);
	}
	
}

/* Memory Access */

void MIPS::ld_st(FuncInstr& instr)                // calls load for loads, store for stores, nothing otherwise
{
	if ( ( instr.pointer == &FuncInstr::lb) || ( instr.pointer == &FuncInstr::lw) || ( instr.pointer == &FuncInstr::lh) || 
	   ( instr.pointer == &FuncInstr::lhu) || ( instr.pointer == &FuncInstr::lbu))
	{
		this->load( instr);
		return;
	}		
	if ( ( instr.pointer == &FuncInstr::sb) || ( instr.pointer == &FuncInstr::sw) || ( instr.pointer == &FuncInstr::sh))
	{
		this->store( instr);
		return;
	}
	return;								
}

/* Writeback */

void MIPS::wb( FuncInstr& instr)
{
	if ( instr.getWB() == 1)
	{
		rf->write( static_cast< RegNum>( instr.get_dst_num_index()), instr.v_dst);
	}
	instr.parseInstr();    // here parseInstr is called to prepare dump    
}

uint32 MIPS::updatePC(const FuncInstr& instr)
{
	this->PC = instr.new_PC;
}

/**
  * Implementation of RF class' methods
  */

/* Constructor of Register File */

RF::RF()
{
	for ( int i = 0; i < MAX_REG; i++)
	{
		reset( static_cast< RegNum>( i));
	}
}

RF::~RF()
{
	
}

/* Read from Register File */

uint32 RF::read( RegNum index) const
{
	if ( ( index != 0) && ( index < MAX_REG))
	{
		return array[ index];	
	}
	else
	{
		return 0;
	}
	
}

/* Write to the Register File */

void RF::write( RegNum index, uint32 data)
{
	if ( ( index != 0) && ( index < MAX_REG))
	{
		array[ index] = data;
		return;
	}
	else
	{
		return;
	}
}

/* Clears Registers to 0 value */

void RF::reset( RegNum index)
{
	{
		if ( static_cast< RegNum>( index) < MAX_REG)
		{
			array[ index] = 0;
			return;
		}
	}
}

/* this method starts process */

void MIPS::run( const string& tr, int instr_to_run)
{   
	/* load trace */
    
	vector< ElfSection> sections_array;
	ElfSection::getAllElfSections( tr.c_str(), sections_array);
	FuncMemory func_mem( tr.c_str(), 32, 10, 12);
	this->mem = &func_mem;
	this->PC = mem->startPC();
    
	/* loop that initializes the process */
	
	for ( int i = 0; i < instr_to_run; ++i) 
	{
		uint32 instr_bytes;
		instr_bytes = fetch();	// fetch
		FuncInstr instr( instr_bytes, this->PC);	// decode
		read_src( instr);	// read sources
		instr.execute();	// execution
		ld_st( instr);	// memory access
		wb( instr);	// writeback 
        updatePC( instr);	// update PC 
		std::cout << instr << endl;	// dump		
    }
}


