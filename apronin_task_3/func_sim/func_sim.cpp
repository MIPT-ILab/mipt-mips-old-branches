/**
 * func_sim.cpp - the module implementing single-cycle MIPS simulator
 * @author Alexander Pronin <alexander.pronin.mipt@gmail.com>
 * Copyright 2015 MIPT-MIPS
 */

// methods of MIPS class

#include <func_sim.h>

MIPS::MIPS() {
    rf = new RF;
    mem = NULL;
    PC = NULL;
}

MIPS::~MIPS() {
    delete rf;
    if ( mem)
        delete mem;
}

// run the program
void MIPS::run( const string& tr, uint32 instr_to_run) {
    mem = new FuncMemory( &( tr[ 0]), (uint64) 32, ( uint64) 10, ( uint64) 12);
    this->PC = mem->startPC();
    for ( uint32 i = 0; i < instr_to_run; i++) {
        uint32 instr_bytes;
        instr_bytes = fetch(); // Fetch
        FuncInstr instr( instr_bytes, PC); // Decode and read sources
        read_reg( instr); // Get values of working registers
        instr.execute(); // Execute
        ld_st( instr); // Memory access
        wb( instr); // Writeback
        updatePC( instr); // Update PC
        std::cout << instr << std::endl; // Dump
    }
}

// load information from memory
void MIPS::load( FuncInstr& instr) {
    switch ( instr.type)
    {
        case FuncInstr::BYTE:
            instr.v_dst = ( int8) mem->read( instr.mem_addr, 1);
            break;
        case FuncInstr::HWORD:
            instr.v_dst = ( int16) mem->read( instr.mem_addr, 2);
            break;
        case FuncInstr::WORD:
            instr.v_dst = ( int32) mem->read( instr.mem_addr, 4);
            break;
        case FuncInstr::UBYTE:
            instr.v_dst = ( uint8) mem->read( instr.mem_addr, 1);
            break;
        case FuncInstr::UHWORD:
            instr.v_dst = ( uint16) mem->read( instr.mem_addr, 2);
            break;
    }
}

// store information in memory
void MIPS::store( const FuncInstr& instr) {
    switch ( instr.type)
    {
        case FuncInstr::BYTE:
            mem->write( instr.v_src1, instr.mem_addr, 1);
            break;
        case FuncInstr::HWORD:
            mem->write( instr.v_src1, instr.mem_addr, 2);
            break;
        case FuncInstr::WORD:
            mem->write( instr.v_src1, instr.mem_addr);
            break;
    }
}

// calls load for loads, store for stores, nothing otherwise
void MIPS::ld_st( FuncInstr& instr) {
    switch ( instr.memOp)
    {
        case FuncInstr::nothing:
            break;
        case FuncInstr::load:
            load( instr);
            break;
        case FuncInstr::store:
            store( instr);
            break;
    }
}

// update PC
void MIPS::updatePC( const FuncInstr& instr) {
    PC = instr.PC_new;
}

// get values of working registers
void MIPS::read_reg( FuncInstr& instr) {
    instr.v_dst = rf->read( instr.i_dst);
    instr.v_src1 = instr.b_src1 = rf->read( instr.i_src1);
    instr.v_src2 = instr.b_src2 = rf->read( instr.i_src2);
}

// save value of dst register
void MIPS::wb(const FuncInstr& instr) {
    rf->write( instr.i_dst, instr.v_dst);
}

// get the operation
uint32 MIPS::fetch() const {
    return mem->read( PC);
}

// methods of RF class

// prepare RF to work
RF::RF() {
    array[ 0] = 0; // set $zero register to 0
}

// get value from register
uint32 RF::read( int index) const {
    return array[ index];
}

// clears register to 0 value
void RF::reset( int index) {
    array[ index] = 0;
}

// write value to register if it is not a $zero register
void RF::write( int index, uint32 data) {
    if ( index != 0)
        array[ index] = data;
}
