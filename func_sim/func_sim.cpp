/**
 * func_sim.cpp - the module implementing single-cycle MIPS simulator
 * @author Alexander Pronin <alexander.pronin.mipt@gmail.com>
 * Copyright 2015 MIPT-MIPS
 */

// methods of MIPS class

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
void MIPS::run( const string& tr, uint instr_to_run) {
    mem = new FuncMemory( tr, 32, 10, 12);
    this->PC = mem->startPC();
    for ( uint i = 0; i < instr_to_run; i++) {
        uint32 instr_bytes;
        instr_bytes = fetch(); // Fetch READY
        FuncInstr instr( instr_bytes); // Decode and read sources READY
        instr.execute(); // Execute READY
        ld_st( instr); // Memory access READY
        wb( instr); // Writeback READY
        updatePC( instr); // Update PC CHECK READY
        std::cout << instr << std::endl; // Dump READY
    }
}

// load information from memory READY
// it may be easier, but it is better than nothing
void MIPS::load( FuncInstr& instr) {
    switch ( instr.type)
    {
        case BYTE:
            instr.v_dst = ( int8) mem->read( instr.mem_addr);
            break;
        case HWORD:
            instr.v_dst = ( int16) mem->read( instr.mem_addr);
            break;
        case WORD:
            instr.v_dst = ( int32) mem->read( instr.mem_addr);
            break;
        case UBYTE:
            instr.v_dst = ( uint8) mem->read( instr.mem_addr);
            break;
        case UHWORD:
            instr.v_dst = ( uint16) mem->read( instr.mem_addr);
            break;
    }
}

// store information in memory READY
void MIPS::store( const FuncInstr& instr) {
    switch ( instr.type)
    {
        case BYTE:
            mem->write( instr.mem_addr, instr.v_dst, 1);
            break;
        case HWORD:
            mem->write( instr.mem_addr, instr.v_dst, 2);
            break;
        case WORD:
            mem->write( instr.mem_addr, instr.v_dst);
            break;
    }
}

// calls load for loads, store for stores, nothing otherwise READY
void MIPS::ld_st( FuncInstr& instr) {
    switch ( instr.memOp)
    {
        case nothing:
            break;
        case load:
            load( instr);
            break;
        case store:
            store( instr);
            break;
    }
}

// update PC READY
void MIPS::updatePC( const FuncInstr& instr) {
    if ( instr.PC_delta)
        PC += instr.PC_delta;
    else if ( instr.jaddr)
        PC = ( PC & 0xF0000000) | instr.jaddr;
    else
        PC = mem->read( instr.v_dst);
}

// get values of working registers READY
void MIPS::read_reg( FuncInstr& instr) {
    instr.v_dst = rf->read( i_dst);
    instr.v_src1 = rf->read( i_src1);
    instr.v_src2 = rf->read( i_src2);
}

// save value of dst register READY
void MIPS::wb(const FuncInstr& instr) {
    rf->write( instr.i_dst, instr.v_dst);
}

// get the operation
uint32 MIPS::fetch() const {
    return mem->read( PC);
}

// methods of RF class

// prepare RF to work READY
RF::RF() {
    array[ 0] = 0; // set $zero register to 0
}

// get value from register READY
uint32 RF::read( RegNum index) const {
    return array[ index];
}

// clears register to 0 value READY
void RF::reset( RegNum index) {
    array[ index] = 0;
}

// write value to register if it is not a $zero register READY
void RF::write( RegNum index, uint32 data) {
    if ( index != 0)
        array[ index] = data;
}