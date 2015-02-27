#include <func_sim.h>
#include <string.h>
#include <strings.h>
#include <func_instr.h>
#include <sstream>

uint32 RF::read(RegNum index) {
    assert(0 <= index && index < MAX_REG);
    return array[index];
}

void RF::write(RegNum index, uint32 data) {
    assert(0 <= index && index < MAX_REG);
    if(index != REG_ZERO) {
        array[index] = data;
    }
}

void RF::reset(RegNum index) {
    assert(0 <= index && index < MAX_REG);
    write(index, 0);
}

std::string RF::dump(std::string indent) const {
    static const char *regTable[] =
            {
                    "zero",
                    "at",
                    "v0", "v1",
                    "a0", "a1", "a2", "a3",
                    "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
                    "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
                    "t8", "t9",
                    "k0", "k1",
                    "gp",
                    "sp",
                    "fp",
                    "ra"
            };
    ostringstream oss;
    for(int i = 0; i < MAX_REG; i++) {
        oss << regTable[i] << indent << std::hex << array[i] << std::dec << std::endl;
    }
    return oss.str();
}

void MIPS::updatePC(FuncInstr const & instr) {
    PC += 4 + instr.PC_delta;
}

MIPS::MIPS() {
    rf = new RF();
}

MIPS::~MIPS() {
    delete rf;
}

void MIPS::run(string const &tr, uint instr_to_run) {
    mem = new Memory(tr.c_str());
    assert(mem != NULL);
    this->PC = (uint32)mem->startPC();
    for(uint i = 0; i < instr_to_run; i++) {
        uint32 instr_bytes = fetch();
        FuncInstr funcInstr(instr_bytes);
        read_src(funcInstr);
        funcInstr.execute();
        ld_st(funcInstr);
        wb(funcInstr);
        updatePC(funcInstr);
        std::cout << funcInstr << std::endl;
    }
    delete mem;
}

void MIPS::read_src(FuncInstr &instr) {
    switch (instr.operation) {
        case FuncInstr::OUT_R_ARITHM:
            instr.v_src1 = rf->read((RegNum) instr.instr.asR.rt);
            instr.v_src2 = rf->read((RegNum) instr.instr.asR.rs);
            break;
        case FuncInstr::OUT_R_SHAMT:
            instr.v_src1 = rf->read((RegNum) instr.instr.asR.rt);
            instr.v_src2 = instr.instr.asR.shamt;
            break;
        case FuncInstr::OUT_R_JUMP:
            instr.v_src2 = rf->read((RegNum) instr.instr.asR.rs);
            break;
        case FuncInstr::OUT_I_ARITHM:
            instr.v_src1 = rf->read((RegNum) instr.rs);
            instr.v_src2 = instr.imm;
            break;
        case FuncInstr::OUT_I_BRANCH:
            instr.v_dst  = rf->read((RegNum) instr.instr.asI.rs);
            instr.v_src1 = rf->read((RegNum) instr.instr.asI.rt);
            instr.v_src2 = instr.instr.asI.imm;
            break;
        case FuncInstr::OUT_J_JUMP:
            instr.v_src2 = instr.instr.asJ.imm;
            break;
        case FuncInstr::OUT_I_LA:
            instr.v_src1 = instr.imm;
            break;
        case FuncInstr::OUT_I_LB:
        case FuncInstr::OUT_I_LW:
            instr.v_src1 = rf->read((RegNum) instr.rs);
            instr.v_src2 = instr.imm;
            break;
        case FuncInstr::OUT_I_SB:
        case FuncInstr::OUT_I_SW:
            instr.v_dst = rf->read((RegNum) instr.rs);
            instr.v_src2 = rf->read((RegNum) instr.rt);
            instr.v_src1 = instr.imm;
            break;
        default:
            std::cerr << "Undknown operation" << instr.operation << std::endl;
            exit(EXIT_FAILURE);
    }
}

void MIPS::wb(FuncInstr &instr) {
    switch (instr.operation) {
        case FuncInstr::OUT_R_ARITHM:
            rf->write((RegNum) instr.instr.asR.rd, instr.v_dst);
            break;
        case FuncInstr::OUT_R_SHAMT:
            rf->write((RegNum) instr.instr.asR.rd, instr.v_dst);
            break;
        case FuncInstr::OUT_R_JUMP:
            break;
        case FuncInstr::OUT_I_ARITHM:
            rf->write((RegNum) instr.instr.asI.rt, instr.v_dst);
            break;
        case FuncInstr::OUT_I_BRANCH:
            break;
        case FuncInstr::OUT_J_JUMP:
            break;
        case FuncInstr::OUT_I_LA:
            rf->write((RegNum) instr.rt, instr.v_dst);
            break;
        case FuncInstr::OUT_I_LB:
        case FuncInstr::OUT_I_LW:
            rf->write((RegNum) instr.rt, instr.v_dst);
            break;
        case FuncInstr::OUT_I_SW:
        case FuncInstr::OUT_I_SB:
            break;
        default:
            std::cerr << "Undknown operation" << instr.operation << std::endl;
            //exit(EXIT_FAILURE);
    }
}

void MIPS::load(FuncInstr &instr) {
    assert(mem != NULL);
    if(instr.operation == FuncInstr::OUT_I_LB) {
        instr.v_dst = (uint8)mem->read(instr.mem_addr, sizeof(uint8));
    }
    else {
        instr.v_dst = (uint32)mem->read(instr.mem_addr);
    }
}

void MIPS::store(const FuncInstr& instr) {
    assert(mem != NULL);
    if(instr.operation == FuncInstr::OUT_I_SB) {
        mem->write(instr.mem_addr, instr.v_src1, sizeof(uint8));
    }
    else {
        mem->write(instr.mem_addr, instr.v_src1);
    }
}

void MIPS::ld_st(FuncInstr& instr) {
    switch (instr.operation) {
        case FuncInstr::OUT_I_LB:
        case FuncInstr::OUT_I_LW:
            load(instr);
            break;
        case FuncInstr::OUT_I_SB:
        case FuncInstr::OUT_I_SW:
            store(instr);
            break;
    }
}

string MIPS::dump(string indent) const {
    ostringstream oss;
    assert(rf != NULL);
    oss << "<------REG DUMP------>" << std::endl;
    oss << rf->dump(indent);
    oss << "<-------------------->" << std::endl;
    return oss.str();
}