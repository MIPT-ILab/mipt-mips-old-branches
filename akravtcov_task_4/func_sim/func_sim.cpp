/*
 * func_sim.cpp - Implementation of functional simulator interface
 * author Alexander Kravtcov
 */

#include <iostream>

#include <cassert>

#include <func_sim.h>
#include <types.h>

const FuncSim::Control FuncSim::controls[] = 
{
    /*          alu_op src1_t dst_t reg_wr imm m_rd m_wr m->reg */
    {   FuncInstr::ADD,     0,    0,     1,  0,   0,   0,     0},
    {  FuncInstr::ADDU,     0,    0,     1,  0,   0,   0,     0},
    {   FuncInstr::SUB,     0,    0,     1,  0,   0,   0,     0},
    {  FuncInstr::SUBU,     0,    0,     1,  0,   0,   0,     0},
    {  FuncInstr::ADDI,     0,    1,     1,  1,   0,   0,     0},
    { FuncInstr::ADDIU,     0,    1,     1,  1,   0,   0,     0},
    {   FuncInstr::SLL,     1,    0,     1,  1,   0,   0,     0},
    {   FuncInstr::SRL,     1,    0,     1,  1,   0,   0,     0},
    {   FuncInstr::BEQ,     0,    0,     0,  0,   0,   0,     0},
    {   FuncInstr::BNE,     0,    0,     0,  0,   0,   0,     0},
    {     FuncInstr::J,     0,    0,     0,  1,   0,   0,     0},
    {    FuncInstr::JR,     0,    0,     0,  0,   0,   0,     0},
    {    FuncInstr::LW,     0,    1,     1,  1,   1,   0,     1},
    {    FuncInstr::SW,     0,    0,     0,  1,   0,   1,     0},
    {   FuncInstr::LUI,     0,    1,     1,  1,   0,   0,     0},
    {   FuncInstr::NOP,     0,    0,     0,  0,   0,   0,     0}
};
FuncSim::FuncSim( const char* executable_file_name)
        : func_memory( executable_file_name), reg_file(), instr( 0)

{
    this->PC = ( uint32)func_memory.startPC();
    this->num_of_executed_instr = 0;
    this->exit_flag = 0;
}

RegFile::Registers FuncSim::muxSrcReg1()
{
    RegFile::Registers src_reg = ( this->current_control.reg_source1_t)
                                   ? this->instr.getRegT()
                                   : this->instr.getRegS();
    return src_reg;
}

RegFile::Registers FuncSim::muxWriteReg()
{
    RegFile::Registers write_reg = ( this->current_control.reg_dest_t)
                                   ? this->instr.getRegT()
                                   : this->instr.getRegD();
    return write_reg;
}

void FuncSim::decode()
{
    assert( !this->exit_flag);

    uint32 bytes= func_memory.read( this->PC);

    this->instr = FuncInstr( bytes);

    // selection control
    int i;
    for ( i = 0; i < FuncInstr::SIZE_OF_INSTR; i++)
    {
        if ( this->controls[ i].alu_op == this->instr.getInstr())
        {
           this->current_control = this->controls[ i];
           break;
        }
    }
    assert( i < FuncInstr::SIZE_OF_INSTR);

    this->imm = this->instr.getImm();
    this->read_reg1 = muxSrcReg1();
    this->read_reg2 = this->instr.getRegT();
    this->write_reg = muxWriteReg();
}

uint64 FuncSim::muxAluSrc()
{
    uint64 source_data = ( this->current_control.alu_src_imm) 
                         ? this->imm 
                         : reg_read_data2;
    return source_data;
}

void FuncSim::readFromReg()
{
    assert( !this->exit_flag);

    this->reg_read_data1 = this->reg_file.read( this->read_reg1);
    this->reg_read_data2 = this->reg_file.read( this->read_reg2);

    this->alu_src_data1 = this->reg_read_data1;
    this->alu_src_data2 = muxAluSrc();
    this->mem_write_data = reg_read_data2;
}

void FuncSim::alu()
{
    assert( !this->exit_flag);

    switch ( this->current_control.alu_op)
    {
        case FuncInstr::ADD:
        case FuncInstr::ADDI:
        case FuncInstr::LW: // compute the effective address
        case FuncInstr::SW:
            this->alu_result = ( uint64)( ( sint64)this->alu_src_data1
                                          + ( sint64)this->alu_src_data2);
            break;

        case FuncInstr::ADDU:
        case FuncInstr::ADDIU:
            this->alu_result = this->alu_src_data1
                               + this->alu_src_data2;
            break;

        case FuncInstr::SUB:
            this->alu_result = ( uint64)( ( sint64)this->alu_src_data1
                                          - ( sint64)this->alu_src_data2);
            break;

        case FuncInstr::SUBU:
            this->alu_result = this->alu_src_data1
                               - this->alu_src_data2;
            break;

        case FuncInstr::SLL:
            this->alu_result = this->alu_src_data1 
                               << this->alu_src_data2;
            break;

        case FuncInstr::SRL:
            this->alu_result = this->alu_src_data1 
                               >> this->alu_src_data2;
            break;

        case FuncInstr::BEQ: // test for equality 
        case FuncInstr::BNE:
            this->alu_result = this->alu_src_data1 
                               - this->alu_src_data2;
            break;

        case FuncInstr::JR:
            this->alu_result = this->alu_src_data1; // it is read data from $s
            break;

        case FuncInstr::LUI: 
            this->alu_result = this->alu_src_data2 
                               << 0x10;
            break;

        case FuncInstr::J: // new PC is calculated in updatePC
        case FuncInstr::NOP:
            break;

        default:
            assert( 0);

    }
    this->alu_zero = ( bool)!this->alu_result;

    this->mem_read_addr = ( uint32)this->alu_result;
    this->mem_write_addr = ( uint32)this->alu_result;
}

void FuncSim::memReadWrite()
{
    if ( this->current_control.mem_read)
    {
        this->mem_read_data = this->func_memory.read( this->mem_read_addr);
    }
    if ( this->current_control.mem_write)
    {
        this->func_memory.write( this->mem_write_data, this->mem_write_addr);
    }
}

uint64 FuncSim::muxWriteDataToRegFile()
{
    uint64 write_data = ( this->current_control.mem_to_reg)
                        ? this->mem_read_data
                        : this->alu_result;
    return write_data;
}

void FuncSim::writeBack()
{
    assert( !this->exit_flag);

    this->reg_write_data = muxWriteDataToRegFile();

    if ( this->current_control.reg_write)
    {
        this->reg_file.write( this->reg_write_data, this->write_reg); 

        if ( this->write_reg == RegFile::V0)
        {
            this->exit_flag = 1;
        }
    }
}

void FuncSim::updatePC()
{
    this->num_of_executed_instr++;

    if ( this->exit_flag)
    {
        return;
    }

    switch ( this->current_control.alu_op)
    {
        case FuncInstr::JR:
            this->PC = ( uint32)this->alu_result;
            break;

        case FuncInstr::J:
            this->PC = ( ( PC & 0xf0000000) | ( 4 * this->imm));
            break;
        
        case FuncInstr::BEQ:
            this->PC = ( this->alu_zero) 
                       ? this->PC + 4 + ( uint32)( 4 * ( sint64)this->imm)
                       : this->PC + 4;
            break;

        case FuncInstr::BNE:
            this->PC = ( !this->alu_zero) 
                       ? this->PC + 4 + ( uint32)( 4 * ( sint64)this->imm)
                       : this->PC + 4;
            break;

        default:
            this->PC += 4;
    }
}

void FuncSim::stopExecution()
{
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "There were executed " << this->num_of_executed_instr 
              << " instructions" << std::endl; 
    std::cout << "The data of $v0 register is " << this->reg_write_data 
              << std::endl;
}

bool FuncSim::getExitFlag()
{
    return this->exit_flag;
}

std::string FuncSim::dumpExecutedInstr()
{
    this->instr.setDataOfRegs( reg_read_data1, reg_read_data2, reg_write_data);
    return this->instr.dump();
}

