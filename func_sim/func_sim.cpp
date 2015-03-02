/**
 * func_sim.cpp - the module implementing single-cycle MIPS simulator
 * @author Alexander Pronin <alexander.pronin.mipt@gmail.com>
 * Copyright 2015 MIPT-MIPS
 */

// methods of MIPS class

// NOT DONE YET
void MIPS::run( const string& tr, uint instr_to_run) {
    // load trace
    this->PC = startPC;
    for ( uint i = 0; i < instr_to_run; i++) {
        uint32 instr_bytes;
        instr_bytes = fetch(); // Fetch
        FuncInstr instr( instr_bytes); // Decode and read sources
        instr.execute(); // Execute CHECK
        ld_st( &instr); // Memory access CHECK
        wb( &instr); // Writeback CHECK
        updatePC( &instr); // Update PC CHECK
        std::cout << instr << std::endl; // Dump
    }
}

// load information from memory
void MIPS::load( FuncInstr& instr) {
        instr.v_dst = mem->read( instr.mem_addr);
    }

// store information in memory
void MIPS::store( const FuncInstr& instr) {
    mem->write( instr.mem_addr, instr.v_dst);
}

// calls load for loads, store for stores, nothing otherwise
void MIPS::ld_st( FuncInstr& instr) {
    // NOT DONE YET
}

// update PC
void MIPS::updatePC( const FuncInstr& instr) {
    PC += instr.PC_delta;
}

void MIPS::read_reg( FuncInstr& instr) {
    instr.v_dst = rf->read( i_dst);
    instr.v_src1 = rf->read( i_src1);
    instr.v_src2 = rf->read( i_src2);
}

// methods of RF class

// prepare RF to work
RF::RF() {
    array[ 0] = 0; // set $zero register to 0
}

// get value from register
uint32 RF::read( RegNum index) const {
    return array[ index];
}

// clears register to 0 value
void RF::reset( RegNum index) {
    array[ index] = 0;
}

// write value to register if it is not a $zero register
void write( RegNum index, uint32 data) {
    if ( index != 0)
        array[ index] = data;
}