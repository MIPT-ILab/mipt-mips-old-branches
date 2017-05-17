#include <func_sim.h>
#include <func_memory.h>
#include <func_instr.h>
#include <string>
#include <iostream>
#include <assert.h>

void RF::read(RegNum index){
    this->resetAll();
    if (index < MAX_REG) return array[index];
    assert(0 <= index && index < MAX_REG);
	return 0;
}

MIPS::MIPS() {
    rf = new RF();
    rf->write( $t0, 5);
}

MIPS::~MIPS() {
    delete rf;
}

void MIPS::updatePC(const FuncInstr& instr) {
    PC = 4 + instr.new_PC;
}

void MIPS::read_src(FuncInstr &instr){
    instr.v_src1 = rf->read(instr.get_src1_num_index());
	instr.v_src2 = rf->read(instr.get_src2_num_index());
    instr.mem_addr = instr.instr.asI.imm;
    instr.jump_address = instr.instr.asJ.imm;

}

void MIPS::wb( FuncInstr &instr){
    switch (instr.format){
        case FuncInstr::FORMAT_R:
            rf->write((RegNum) instr.instr.asR.rd, instr.v_dst);
            break;
        case FuncInstr::FORMAT_I:
            rf->write((RegNum) instr.instr.asI.r), instr.v_dst);
            break;
        default:
            break;
    }


void MIPS::run( const string& fileName , uint32 instr_to_run){
    mem = new FuncMemory(fileName.c_str());
    assert(mem != NULL);

    this->PC = mem->startPC();

    for (uint32 i = 0; i < instr_to_run; ++i) {
        uint32 instr_bytes = fetch();
        FuncInstr instr(instr_bytes, PC);
        read_src(instr);
        instr.execute();
        ld_st(instr);
        wb(instr);
        updatePC(instr);
        cout << instr.Dump("\n");
    }
    delete mem;
}

void MIPS::load(FuncInstr &instr) {
    if(instr.operation == FuncInstr::OUT_I_LB) {
    instr.v_dst = (uint8)mem->read(instr.mem_addr);
    }
    else {
    instr.v_dst = (uint16)mem->read(instr.mem_addr + instr.v_src1);
}

void MIPS::store(const FuncInstr &instr) {
    if(instr.operation == FuncInstr::OUT_I_SB) {
    mem->write(instr.mem_addr, instr.v_src1, sizeof(uint8));
    }
    else {
        mem->write(instr.mem_addr, instr.v_src1);
    }
}


void MIPS::ld_st(FuncInstr &instr)
{
        if(instr.is_load())load(instr);
        if(instr.is_store())store(instr);
        else store(instr);
}


