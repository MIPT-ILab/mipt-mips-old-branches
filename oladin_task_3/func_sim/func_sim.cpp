/**
 * func_sim.cpp
 * Emplementation of the MIPS functional simulator.
 * MIPT-MIPS Assignment 3.
 * Ladin Oleg.
 */

/* Generic C. */
#include <cstdlib>

/* Generic C++. */
#include <sstream>
#include <iomanip>

/* Functional simulator modules. */
#include <func_sim.h>



/** Implementation of FuncInstr methods, requaried for func. simulator. */
int FuncInstr::get_src1_num_index() const
{
    switch ( isaTable[ isaNum].operation)
    {
        case OUT_R_ARITHM:
        case OUT_R_JUMP:
        case OUT_R_LOG:
        case OUT_I_ARITHM:
        case OUT_I_LOG:
        case OUT_I_BRANCH:
        case OUT_I_MEM:
            return instr.asR.rs;
        case OUT_R_SHAMT:
        case OUT_I_LOAD:
        case OUT_J_JUMP:
            return REG_MAX; // don't need to read (or immediate)
        default: // error
            cerr << "ERROR: Wrong instruction!\n";
            exit( EXIT_FAILURE);
    }
}

int FuncInstr::get_src2_num_index() const
{
    switch ( isaTable[ isaNum].operation)
    {
        case OUT_R_ARITHM:
        case OUT_R_SHAMT:
        case OUT_R_LOG:
        case OUT_I_BRANCH:
            return instr.asR.rt;
        case OUT_R_JUMP:
        case OUT_I_ARITHM:
        case OUT_I_LOAD:
        case OUT_I_LOG:
        case OUT_I_MEM:
        case OUT_J_JUMP:
            return REG_MAX; // don't need to read (or immediate)
        default: // error
            cerr << "ERROR: Wrong instruction!\n";
            exit( EXIT_FAILURE);
    }
}

int FuncInstr::get_dst_num_index() const
{
    switch ( isaTable[ isaNum].operation)
    {
        case OUT_R_ARITHM:
        case OUT_R_SHAMT:
        case OUT_R_LOG:
            return instr.asR.rd;
        case OUT_I_ARITHM:
        case OUT_I_LOAD:
        case OUT_I_LOG:
        case OUT_I_MEM:
            return instr.asR.rt;
        case OUT_I_BRANCH:
        case OUT_R_JUMP:
        case OUT_J_JUMP:
            return REG_MAX; // don't need to write
        default: // error
            cerr << "ERROR: Wrong instruction!\n";
            exit( EXIT_FAILURE);
    }
}

/* Executes appropriate micromethod.*/
void FuncInstr::execute()
{
    ( this->*func)();
}



MIPS::MIPS()
{
    /**
     * We have to allocate memory only for RF, FuncMemory has special
     * constructor.
     */
    rf = new RF;
    rf->reset( REG_ZERO); // zeroing ZERO register
}

MIPS::~MIPS()
{
    delete rf;
}

void MIPS::run( const string& tr, unsigned int instr_to_run)
{
    /* Create FuncMemory with standard sizes. */
    mem = new FuncMemory( tr.data(), 32, 10, 12);
    PC = mem->startPC(); // set PC at the beginning
    uint32 instr_bytes;
    for ( unsigned int i = 0; i < instr_to_run; ++i) // main loop
    {
        instr_bytes = fetch(); // fetch
        FuncInstr instr( instr_bytes, PC); // decode
        read_src( instr); // read sources
        instr.execute(); // execute
        ld_st( instr); // memory access
        wb( instr); // writeback
        updatePC( instr); // update PC
        cout << dmp( instr) << endl; // runtime disassembler dump
    }
    delete mem;
}

/* Fetch instruction. */
uint32 MIPS::fetch() const
{
    return mem->read( PC); // load new instruction
}

/* Read from RF, if needed. */
void MIPS::read_src( FuncInstr& instr)
{
    if ( instr.get_src1_num_index() != REG_MAX) // if need to read source 1
    {
        instr.v_src1 = rf->read( static_cast< RegNum>( instr.get_src1_num_index()));
    }
    if ( instr.get_src2_num_index() != REG_MAX) // if need to read source 2
    {
        instr.v_src2 = rf->read( static_cast< RegNum>( instr.get_src2_num_index()));
    }
}

/* Checks if instruction access memory. And calls load or store method. */
void MIPS::ld_st( FuncInstr& instr)
{
    if ( instr.prop.bit.mw) // if instr writes into memory
    {
        store( instr);
    } else if ( instr.prop.bit.mr) // if instr reads from memory
    {
        load( instr);
    }
}

/* Read from memory. */
void MIPS::load( FuncInstr& instr)
{
    if ( instr.prop.bit.mrt == 0) // "lb" or "lbu"
    {
        if ( instr.prop.bit.mrs == 0) // "lbu"
        {
            instr.v_dst = static_cast< uint8>( mem->read( instr.mem_addr, 1));
        } else // "lb"
        {
            instr.v_dst = static_cast< int8>( mem->read( instr.mem_addr, 1));
        }
    } else if ( instr.prop.bit.mrt == 1) // "lh" or "lhu"
    {
        if ( instr.prop.bit.mrs == 0) // "lhu"
        {
            instr.v_dst = static_cast< uint16>( mem->read( instr.mem_addr, 2));
        } else // "lh"
        {
            instr.v_dst = static_cast< int16>( mem->read( instr.mem_addr, 2));
        }
    } else // "lw"
    {
        instr.v_dst = mem->read( instr.mem_addr);
    }
}

/* Write into memory. */
void MIPS::store( const FuncInstr& instr)
{
    if ( instr.prop.bit.mwt == 0) // "sb"
    {
        mem->write( static_cast< uint8>( instr.v_dst), instr.mem_addr, 1);
    } else if ( instr.prop.bit.mwt == 1) // "sh"
    {
        mem->write( static_cast< uint16>( instr.v_dst), instr.mem_addr, 2);
    } else // "sw"
    {
        mem->write( instr.v_dst, instr.mem_addr);
    }
}

/* Write into RF. */
void MIPS::wb( const FuncInstr& instr)
{
    if ( !instr.prop.bit.rnw) // if instr WRITES into RF
    {
        rf->write( static_cast< RegNum>( instr.get_dst_num_index()),
                                         instr.v_dst);
    }
}

/* Update programm counter. */
void MIPS::updatePC( const FuncInstr& instr)
{
    PC = instr.new_PC;
}

/** Runtime disassembler dump (parsing instr.Dump() method return string). */
string MIPS::dmp( const FuncInstr& instr)
{
    ostringstream oss; // output stream
    string str = instr.Dump( "").data(); // get string
    /** Cycle parsing string by byte, searching for "[" - place for value. */
    for ( int i = 0; i < str.size(); ++i)
    {
        if ( str[ i] == '[') // if reachs "["
        {
            oss << "[0x" << hex << setfill( '0') << setw( 8);
            if ( str[ i + 3] == 's') // put value from first source register
            {
                oss << instr.v_src1;
                i += 4; // skip
            } else if ( str[ i + 3] == 't') // from second source register
            {
                oss << instr.v_src2;
                i += 4; // skip
            } else // get calculated value from RF
            {
                oss << rf->read( static_cast< RegNum>( ( ( str[ i + 1] - '0')
                                                         * 10 + str[ i + 2] - '0')));
                i += 3; // skip
            }
            oss << dec << "]";
        } else // another symbol, just put it
        {
            oss << str[ i];
        }
    }
    return oss.str();
}



uint32 RF::read( RegNum index) const
{
    if ( ( index < REG_ZERO) || ( index >= REG_MAX)) // check register number
    {
        cerr << "ERROR: Wrong register number!\n";
        exit( EXIT_FAILURE);
    }
    return array[ index];
}

void RF::write( RegNum index, uint32 data)
{
    if ( ( index < REG_ZERO) || ( index >= REG_MAX)) // check register number
    {
        cerr << "ERROR: Wrong register number!\n";
        exit( EXIT_FAILURE);
    }
    if ( index == REG_ZERO) // ZERO is unchangeable!
    {
        return;
    } else
    {
        array[ index] = data;
    }
}

void RF::reset( RegNum index)
{
    if ( ( index < REG_ZERO) || ( index >= REG_MAX)) // check register number
    {
        cerr << "ERROR: Wrong register number!\n";
        exit( EXIT_FAILURE);
    }
    array[ index] = 0;
}
