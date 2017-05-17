/**
 * func_sim.cpp - The methods for the concept of
 * MIPS single-cycle implementation.
 * @author Mikhail Lyubogoschev <lyubogoshchev@phystech.edu>
 * Copyright 2015 uArchSim iLab project
 */

#include <func_sim.h>


MIPS::MIPS():
    PC( 0L),
    mem( NULL)
{
    rf = new RF;
}

void RF::write( RegNum index, uint32 data)
{
    if( ZERO < index && index < MAXREG )
        regArr[ index] = data;
    else 
        cerr << "Incorrect Register\n";
}

void MIPS::readSrc( FuncInstr& instr)
{
    if (instr.getFormat() != FuncInstr::FORMAT_J)
    {
        instr.v_src = rf->read(instr.get_src_num_index());
        instr.v_tgt = rf->read(instr.get_tgt_num_index());
        instr.PC_delta = 4;
    } else {
        instr.PC_delta = (PC & 0xf0000000) | (instr.cnst << 2);
	}
}

void MIPS::ld_st(FuncInstr& instr)
{
    if ( instr.getFormat() != FuncInstr::FORMAT_I)
        return;
    switch ( instr.getIOpcode())
    {
    case 0x20:
        load( instr, 1);
        break;
    case 0x23:
        load( instr);
        break;
    case 0x28:
        store( instr, 1);
        break;
    case 0x2b:
        store( instr);
        break;
	case 0x21:
		load( instr, 2);
		break;
	case 0x24:
		loadu( instr);
		break;
	case 0x25:
		loadu( instr, 2);
		break;
	case 0x29:
		store( instr, 2);
		break;
    }
}

void MIPS::wb( FuncInstr& instr)
{
    switch ( instr.operation)
    {
        case FuncInstr::R_ARITHM:
            rf->write( instr.get_dst_num_index(), instr.v_dst);
            break;
        case FuncInstr::I_ARITHM:
            rf->write( instr.get_tgt_num_index(), instr.v_tgt);
            break;
        case FuncInstr::I_LUI:
            rf->write( instr.get_tgt_num_index(), instr.cnst << 16);
            break;
    }
}

void MIPS::updatePC( const FuncInstr& instr)
{
    switch( instr.operation)
    {
        case FuncInstr::R_JUMP:
            PC = instr.v_src;
            break;
        case FuncInstr::J_JUMP:
            PC = ( uint64)instr.PC_delta;
            break;
        default:
            PC += instr.PC_delta;
			break;
     }
}

void MIPS::dump( FuncInstr& instr)
{
    cout << instr << endl;
}
void MIPS::run( const string& mem_copy, uint32 instr_to_run)
{
    mem = new FuncMemory( mem_copy.data());
    assert( mem);
    PC = mem->startPC();
    uint32 instr_bytes = 0;
    for ( uint32 i = 0; i < instr_to_run; i++)
    {
        instr_bytes = fetch();
        FuncInstr instr( instr_bytes);
        readSrc( instr);
        instr.execute();
        ld_st( instr);
        wb( instr);
        updatePC( instr);
        dump( instr);
    }
}
