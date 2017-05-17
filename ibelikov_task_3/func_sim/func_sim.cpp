#include <func_sim.h>
#include <func_memory.h>
#include <func_instr.h>
#include <string>
#include <iostream>

RF::RF(){
    this->resetAll();
}

uint32 MIPS::fetch() const {
    return mem->read( PC, 4);
}

void MIPS::updatePC(const FuncInstr& instr){
    PC = instr.new_PC;
}

MIPS::MIPS(){
    rf = new RF();
    rf->write( $t0, 5);
}

uint32 MIPS::read(RegNum index){
    return rf->read(index);
}

void MIPS::read_src( FuncInstr& instr){
    instr.v_src1 = rf->read( static_cast<RegNum>(instr.instr.asR.rs));
    instr.v_src2 = rf->read( static_cast<RegNum>(instr.instr.asR.rt));
    instr.mem_addr = instr.instr.asI.imm;
    instr.jump_address = instr.instr.asJ.imm;
}

void MIPS::wb( FuncInstr& instr){
    switch (instr.format){
        case FuncInstr::FORMAT_R:
            rf->write( static_cast<RegNum>(instr.instr.asR.rd), instr.v_dst);
            break;
        case FuncInstr::FORMAT_I:
            rf->write( static_cast<RegNum>(instr.instr.asI.rt), instr.v_dst);
            break;
        default:
            break;
    }
}

void MIPS::run( const string& executable_file_name , uint32 instr_to_run){
    // load trace
    mem = new FuncMemory(executable_file_name.c_str());

    PC = mem->startPC();

    for (uint32 i = 0; i < instr_to_run; ++i) {
        uint32 instr_bytes = fetch();
        // Fetch
        FuncInstr instr(instr_bytes, PC);
        // Decode and read sources
        read_src(instr);
        // Execute
        instr.execute();
        // Memory access
        ld_st(instr);
        // Writeback
        wb(instr);
        // Update PC
        updatePC(instr);
        // Dump
        cout << instr.Dump("\n");
    }
}

void MIPS::load( FuncInstr& instr) {
    switch (instr.get_ldst_size()){
        case FuncInstr::BYTE:
            instr.v_dst = mem->read(instr.mem_addr + instr.v_src1, 1);
            break;
        case FuncInstr::HWORD:
            instr.v_dst = mem->read(instr.mem_addr + instr.v_src1, 2);
            break;
        case FuncInstr::WORD:
            instr.v_dst = mem->read(instr.mem_addr + instr.v_src1, 4);
            break;
        case FuncInstr::UBYTE:
            instr.v_dst = (uint8)mem->read(instr.mem_addr + instr.v_src1, 1);
            break;
        case FuncInstr::UHWORD:
            instr.v_dst = (uint16)mem->read(instr.mem_addr + instr.v_src1, 2);
            break;
        default:
            break;
    }
}

void MIPS::store( const FuncInstr& instr) {
    switch (instr.get_ldst_size()){
        case FuncInstr::BYTE:
            mem->write( instr.mem_addr + instr.v_src1, (uint8)instr.v_src2, 1);
            break;
        case FuncInstr::HWORD:
            mem->write( instr.mem_addr + instr.v_src1, (uint8)instr.v_src2, 2);
            break;
        case FuncInstr::WORD:
            mem->write( instr.mem_addr + instr.v_src1, (uint8)instr.v_src2, 4);
            break;
        default:
            break;
    }
}

void MIPS::ld_st( FuncInstr& instr) {
    if(instr.is_load())load(instr);
    if(instr.is_store())store(instr);
    // calls load for loads, store for stores, nothing otherwise
}
