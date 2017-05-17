/**
 * func_sim.cpp - module implementing 
 * functional simulator of MIPS
 * @author Pavel Poroshin <pavel.a.poroshin@gmail.com>
 * Copyright 2013 uArchSim iLab project
 */

// Genereic C
#include <stdlib.h>
#include <cassert>

// Generic C++
#include <iostream>
#include <string>
#include <sstream>

// uArchSim modules
#include <func_sim.h>
#include <reg_file.h>
#include <func_instr.h>
#include <func_memory.h>
#include <types.h>

ProgramExecuter::ProgramExecuter( const char* executable_file_name)
{
    assert( executable_file_name != NULL);
    program_name = executable_file_name;
    mem = new FuncMemory( executable_file_name);
    reg_file = new RegFile( 32);
    cur_instr = NULL;
    pc = mem->startPC();
    is_terminated = false;
}

ProgramExecuter::~ProgramExecuter()
{
    delete mem;
    delete reg_file;
    if( cur_instr != NULL)
    {
        delete cur_instr;
    }
}

void ProgramExecuter::execInstr()
{
    fetch();
    decode();
    execute();
    memoryOp();
    writeBack();
}

void ProgramExecuter::updatePC()
{
    pc = next_pc;
}

bool ProgramExecuter::isTerminated()
{
    return is_terminated;
}

uint32 ProgramExecuter::getTermStatus()
{
    return term_status;
}

std::string ProgramExecuter::dump( std::string indent)
{
    if( cur_instr == NULL)
    {
        return "";
    }
    std::ostringstream oss;
    oss << std::hex << indent << "0x" << pc << " " << cur_instr->getName() << " ";
    switch( cur_instr->getPrintType())
    {
        case FuncInstr::PT_DST_S:
            oss << cur_instr->getDRegName() << "(0x" << ( sint32)d_val <<  "), " << cur_instr->getSRegName() 
                << "(0x" << ( sint32)s_val << "), " << cur_instr->getTRegName() << "(0x" << ( sint32)t_val << ")";
            break;

        case FuncInstr::PT_DST_U:
            oss << cur_instr->getDRegName() << "(0x" << d_val <<  "), " << cur_instr->getSRegName() 
                << "(0x" << s_val << "), " << cur_instr->getTRegName() << "(0x" << t_val << ")";
            break;

        case FuncInstr::PT_TSI_S:
            oss << cur_instr->getTRegName() << "(0x" << ( sint32)t_val <<  "), " << cur_instr->getSRegName() 
                << "(0x" << ( sint32)s_val << "), 0x" << ( sint16)immed_val;
            break;

        case FuncInstr::PT_TSI_U:
            oss << cur_instr->getTRegName() << "(0x" << t_val <<  "), " << cur_instr->getSRegName() 
                << "(0x" << s_val << "), 0x" << immed_val;
            break;

        case FuncInstr::PT_DTSH_U:
            oss << cur_instr->getDRegName() << "(0x" << d_val <<  "), " << cur_instr->getTRegName() 
                << "(0x" << t_val <<  "), 0x" << shamt_val;
            break;

        case FuncInstr::PT_STI_S:
            oss << cur_instr->getSRegName() << "(0x" << ( sint32)s_val <<  "), " << cur_instr->getTRegName() 
                << "(0x" << ( sint32)t_val <<  "), 0x" << ( sint32)immed_val;
            break;

        case FuncInstr::PT_ADDR_U:
            oss << "0x" << addr_val;
            break;

        case FuncInstr::PT_S_U:
            oss << cur_instr->getSRegName() << "(0x" << s_val << ")";
            break;

        case FuncInstr::PT_TS_U:
            oss << cur_instr->getTRegName() << "(0x" << t_val <<  "), " << cur_instr->getSRegName() 
                << "(0x" << s_val <<  ")";
            break;

        case FuncInstr::PT_T_U:
            oss << cur_instr->getTRegName() << "(0x" << t_val <<  ")";
            break;

        case FuncInstr::PT_NAME:
            break;

        case FuncInstr::PT_TIS_S:
            oss << cur_instr->getTRegName() << "(0x" << t_val <<  "), 0x" << immed_val << "(" 
                << cur_instr->getSRegName() << "(0x" << s_val <<  "))";
            break;

        case FuncInstr::PT_TI_U:
            oss << cur_instr->getTRegName() << "(0x" << t_val <<  "), 0x" << immed_val;
            break;
        default:
            std::cerr << "ERROR! Unsupporeted print type occured!" << std::endl;
            exit( EXIT_FAILURE);
    }
    return oss.str();
}

void ProgramExecuter::fetch()
{
    cur_instr_code = mem->read( pc);
}

void ProgramExecuter::decode()
{
    if( cur_instr != NULL) 
    {
        delete cur_instr;
    }
    cur_instr = new FuncInstr( cur_instr_code);
    switch( cur_instr->getType())
    {
        case FuncInstr::TYPE_R:
            shamt_val = cur_instr->getShamt();
            d_val     = reg_file->read( cur_instr->getDReg());
            t_val     = reg_file->read( cur_instr->getTReg());
            s_val     = reg_file->read( cur_instr->getSReg());
            break;
        case FuncInstr::TYPE_I:
            immed_val = cur_instr->getImmed();
            t_val     = reg_file->read( cur_instr->getTReg());
            s_val     = reg_file->read( cur_instr->getSReg());
            break;
        case FuncInstr::TYPE_J:
            addr_val = cur_instr->getAddr();
            break;
    }
}

void ProgramExecuter::execute()
{
    next_pc = pc + 4;
    switch( cur_instr->getInstr())
    {
        case FuncInstr::INSTR_ADD:
            alu_res = ( sint32)t_val + ( sint32)s_val;
            break;

        case FuncInstr::INSTR_ADDU:
            alu_res = t_val + s_val;
            break;

        case FuncInstr::INSTR_SUB:
            alu_res = ( sint32)t_val - ( sint32)s_val;
            break;

        case FuncInstr::INSTR_SUBU:
            alu_res = t_val - s_val;
            break;

        case FuncInstr::INSTR_ADDI:
            alu_res = ( sint32)s_val +  ( sint16)immed_val;
            break;

        case FuncInstr::INSTR_ADDIU:
            alu_res = s_val + immed_val;
            break;

        case FuncInstr::INSTR_SLL:
            alu_res = t_val << shamt_val;
            break;

        case FuncInstr::INSTR_SRL:
            alu_res = t_val >> shamt_val;
            break;

        case FuncInstr::INSTR_BEQ:
            alu_res = s_val - t_val;
            if( alu_res == 0)
            {
                next_pc = next_pc + 4 * ( sint16)immed_val;
            }
            break;

        case FuncInstr::INSTR_BNE:
            alu_res = s_val - t_val;
            if( alu_res != 0)
            {
                next_pc = next_pc + 4 * ( sint16)immed_val;
            }
            break;

        case FuncInstr::INSTR_J:
            next_pc = ( pc & 0xf0000000) | (4 * addr_val); 
            break;

        case FuncInstr::INSTR_JR:
            next_pc = s_val; 
            break;

        case FuncInstr::INSTR_LW:
            alu_res = ( sint32)s_val + ( sint16)immed_val;
            break;

        case FuncInstr::INSTR_SW:
            alu_res = ( sint32)s_val + ( sint16)immed_val;
            break;

        case FuncInstr::INSTR_LUI:
            alu_res = immed_val << 0x10;
            break;

        default:
            std::cerr << "ERROR! Unsupporeted instruction occured!" << std::endl;
            exit( EXIT_FAILURE);
    }
}

void ProgramExecuter::memoryOp()
{
    switch( cur_instr->getInstr())
    {
        case FuncInstr::INSTR_LW:
            load_res = mem->read( alu_res);
            break;
        case FuncInstr::INSTR_SW:
            mem->write( t_val, alu_res);
            break;
    }
}

void ProgramExecuter::writeBack()
{
    uint8 wb_reg;
    switch( cur_instr->getInstr())
    {
        case FuncInstr::INSTR_ADD:
        case FuncInstr::INSTR_ADDU:
        case FuncInstr::INSTR_SUB:
        case FuncInstr::INSTR_SUBU:
        case FuncInstr::INSTR_SLL:
        case FuncInstr::INSTR_SRL:
            wb_reg = cur_instr->getDReg();
            d_val = alu_res;
            reg_file->write( wb_reg, d_val);
            if( wb_reg == termination_reg)
            {
                is_terminated = true;
                term_status = d_val;
            }
            break;

        case FuncInstr::INSTR_ADDI:
        case FuncInstr::INSTR_ADDIU:
        case FuncInstr::INSTR_LUI:
            wb_reg = cur_instr->getTReg();
            if( wb_reg == FuncInstr::REG_ZERO)
            {
                std::cerr << "ERROR! Invalid instruction: writing to the \"zero\" register!" << std::endl;
                exit( EXIT_FAILURE);
            }
            t_val = alu_res;
            reg_file->write( wb_reg, t_val);
            if( wb_reg == termination_reg)
            {
                is_terminated = true;
                term_status = t_val;
            }
            break;

        case FuncInstr::INSTR_LW:
            wb_reg = cur_instr->getTReg();
            if( wb_reg == FuncInstr::REG_ZERO)
            {
                std::cerr << "ERROR! Invalid instruction: writing to the \"zero\" register!" << std::endl;
                exit( EXIT_FAILURE);
            }
            t_val = load_res;
            reg_file->write( wb_reg, t_val);
            if( wb_reg == termination_reg)
            {
                is_terminated = true;
                term_status = t_val;
            }
            break;
    }
}