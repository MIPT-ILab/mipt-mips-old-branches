/**
 * func_sim.cpp
 * @author Yury Samarin <yuri.a.samarin@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C
#include <cassert>
#include <cstdlib>

// Generic C++
#include <iostream>
#include <string> 
#include <sstream>

// uArchSim modules
#include <func_sim.h>
#include <func_instr.h>
#include <reg_file.h>
#include <func_memory.h>

using namespace std;

FuncSim::FuncSim( const char *elf_file)
{
    instr_count = 0;
    mem_file = new FuncMemory( elf_file);
    PC = mem_file->startPC();
    for_dump << hex;
    for_dump.setf( for_dump.showbase);
    dump_state = true;
}

FuncInstr FuncSim::decodeInstr( uint32 bytes)
{
	return FuncInstr( bytes);
}

void FuncSim::execAdd(  const FuncInstr& instr)
{
    uint32 tmp1 = reg_data[ instr.getTarget()];
    uint32 tmp2 = reg_data[ instr.getSource()];
    reg_data[ instr.getDest()] = tmp1 + tmp2; 
    for_dump << "add " << FuncInstr::REGNAME[ instr.getDest()] 
             << "(" << reg_data[ instr.getDest()] << ")"  << ", " 
             << FuncInstr::REGNAME[ instr.getSource()] 
             << "(" << tmp2 << ")" << ", " 
             << FuncInstr::REGNAME[ instr.getTarget()]
             << "(" << tmp1 << ")";
}

void FuncSim::execAddu( const FuncInstr& instr)
{
    uint32 tmp1 = reg_data[ instr.getTarget()];
    uint32 tmp2 = reg_data[ instr.getSource()];
    reg_data[ instr.getDest()] = uint32( tmp1 + tmp2); 
    for_dump << "addu " << FuncInstr::REGNAME[ instr.getDest()] 
             << "(" << reg_data[ instr.getDest()] << ")"  << ", " 
             << FuncInstr::REGNAME[ instr.getSource()] 
             << "(" << tmp2 << ")" << ", " 
             << FuncInstr::REGNAME[ instr.getTarget()]
             << "(" << tmp1 << ")";
}

void FuncSim::execSub(  const FuncInstr& instr)
{
    uint32 tmp1 = reg_data[ instr.getTarget()];
    uint32 tmp2 = reg_data[ instr.getSource()];
    reg_data[ instr.getDest()] = tmp1 - tmp2; 
    for_dump << "sub " << FuncInstr::REGNAME[ instr.getDest()] 
             << "(" << reg_data[ instr.getDest()] << ")"  << ", " 
             << FuncInstr::REGNAME[ instr.getSource()] 
             << "(" << tmp2 << ")" << ", " 
             << FuncInstr::REGNAME[ instr.getTarget()]
             << "(" << tmp1 << ")";
}

void FuncSim::execSubu( const FuncInstr& instr)
{
    uint32 tmp1 = reg_data[ instr.getTarget()];
    uint32 tmp2 = reg_data[ instr.getSource()];
    reg_data[ instr.getDest()] = uint32( tmp1 - tmp2); 
    for_dump << "subu " << FuncInstr::REGNAME[ instr.getDest()] 
             << "(" << reg_data[ instr.getDest()] << ")"  << ", " 
             << FuncInstr::REGNAME[ instr.getSource()] 
             << "(" << tmp2 << ")" << ", " 
             << FuncInstr::REGNAME[ instr.getTarget()]
             << "(" << tmp1 << ")";
}

void FuncSim::execSll(  const FuncInstr& instr)
{
    uint32 tmp = reg_data[ instr.getTarget()];
    reg_data[ instr.getDest()] = tmp << instr.getShamt(); 
    for_dump <<"sll "<< FuncInstr::REGNAME[ instr.getDest()] 
             << "("  << reg_data[ instr.getDest()] << ")" 
             << ", " << FuncInstr::REGNAME[ instr.getTarget()]
             << "("  << tmp << ")" << ", " << instr.getShamt();
}

void FuncSim::execSrl(  const FuncInstr& instr)
{
    uint32 tmp = reg_data[ instr.getTarget()];
    reg_data[ instr.getDest()] = tmp >> instr.getShamt(); 
    for_dump <<"srl "<< FuncInstr::REGNAME[ instr.getDest()] 
             << "("  << reg_data[ instr.getDest()] << ")" 
             << ", " << FuncInstr::REGNAME[ instr.getTarget()]
             << "("  << tmp << ")" << ", " << instr.getShamt();
}

void FuncSim::execJr(   const FuncInstr& instr)
{
    PC = reg_data[ instr.getSource()];
    for_dump << "jr "<< FuncInstr::REGNAME[ instr.getSource()]
             << "("  << reg_data[ instr.getSource()] << ")";
}

void FuncSim::execAddi( const FuncInstr& instr)
{
    uint32 tmp = reg_data[ instr.getSource()];
    reg_data[ instr.getTarget()] = tmp + instr.getImmed(); 
    for_dump << "addi " << FuncInstr::REGNAME[ instr.getTarget()] 
             << "(" << reg_data[ instr.getTarget()] << ")" << ", " 
             << FuncInstr::REGNAME[ instr.getSource()] 
             << "(" << tmp << ")" << ", " << instr.getImmed();
}

void FuncSim::execAddiu(const FuncInstr& instr)
{
    uint32 tmp = reg_data[ instr.getSource()];
    reg_data[ instr.getTarget()] = uint32( tmp + instr.getImmed()); 
    for_dump << "addiu " << FuncInstr::REGNAME[ instr.getTarget()] 
             << "(" << reg_data[ instr.getTarget()] << ")" << ", " 
             << FuncInstr::REGNAME[ instr.getSource()] 
             << "(" << tmp << ")" << ", " << instr.getImmed();
}

void FuncSim::execBeq(  const FuncInstr& instr)
{
    ( reg_data[ instr.getSource()] == reg_data[ instr.getTarget()])
        ? PC = PC + 4 + 4 * (sint16)instr.getImmed(): PC = PC + 4;
    for_dump <<"beq "<< FuncInstr::REGNAME[ instr.getSource()]
             << "("  << reg_data[ instr.getSource()] << ")"
             << ", " << FuncInstr::REGNAME[ instr.getTarget()]
             << "("  << reg_data[ instr.getTarget()] << ")"
             << ", " << instr.getImmed();
}

void FuncSim::execBne(  const FuncInstr& instr)
{
    ( reg_data[ instr.getSource()] != reg_data[ instr.getTarget()])
        ? PC = PC + 4 + 4 * (sint16)instr.getImmed(): PC = PC + 4;
    for_dump <<"bne "<< FuncInstr::REGNAME[ instr.getSource()]
             << "("  << reg_data[ instr.getSource()] << ")"
             << ", " << FuncInstr::REGNAME[ instr.getTarget()]
             << "("  << reg_data[ instr.getTarget()] << ")"
             << ", " << instr.getImmed();
}

void FuncSim::execJump( const FuncInstr& instr)
{
    PC = (PC & 0xf0000000) | (sint32)( instr.getJump() * 4); 
    for_dump << "j " << instr.getJump();
}

void FuncSim::execSw(   const FuncInstr& instr)
{
    mem_file->write( reg_data[ instr.getTarget()], reg_data[ instr.getSource()] + instr.getImmed());
    for_dump << "sw " << FuncInstr::REGNAME[ instr.getTarget()]
             << "("   << reg_data[ instr.getTarget()] << ")"
             << ", "  << instr.getImmed() << "("
             << FuncInstr::REGNAME[ instr.getSource()] << "("
             << reg_data[ instr.getSource()] << ")" << ")";
}

void FuncSim::execLw(   const FuncInstr& instr)
{
    uint32 tmp = reg_data[ instr.getSource()];
    reg_data[ instr.getTarget()] = mem_file->read( tmp + instr.getImmed());
    for_dump << "lw " << FuncInstr::REGNAME[ instr.getTarget()]
             << "("   << reg_data[ instr.getTarget()] << ")"
             << ", "  << instr.getImmed() << "("
             << FuncInstr::REGNAME[ instr.getSource()] << "("
             << tmp << ")" << ")";
}

void FuncSim::execLui(  const FuncInstr& instr)
{
    reg_data[ instr.getTarget()] = (sint16)instr.getImmed() << 0x10;
    for_dump <<"lui "<< FuncInstr::REGNAME[ instr.getTarget()]
             << "("  << reg_data[ instr.getTarget()] << ")" << ", "
             << instr.getImmed();
}

void FuncSim::execNop( const FuncInstr& instr)
{
    for_dump << "nop";
}

void FuncSim::execMove( const FuncInstr& instr)
{
    reg_data[ instr.getDest()] = reg_data[ instr.getSource()];
    for_dump << "move " << FuncInstr::REGNAME[ instr.getDest()]
             << "("     << reg_data[ instr.getDest()] << ")"
             << ", "    << FuncInstr::REGNAME[ instr.getSource()]
             << "("     << reg_data[ instr.getSource()] << ")";
}    

void FuncSim::execClear( const FuncInstr& instr)
{
    reg_data[ instr.getTarget()] = 0;
    for_dump << "clear " << FuncInstr::REGNAME[ instr.getTarget()]
             << "("      << reg_data[ instr.getTarget()] << ")";
}

void FuncSim::execInstr( const FuncInstr& instr)
{
    if ( instr.is_nop())
    {
        execNop( instr);
        PC = PC + 4;
        return;
    }
    if ( instr.is_move())
    {
        execMove( instr);
        PC = PC + 4;
        return;
    }
    if ( instr.is_clear())
    {
        execClear( instr);
        PC = PC + 4;
        return;
    }
    switch( instr.getType())
    {
        case FuncInstr::R_TYPE:
            switch( instr.getFunct())
            {
                case FuncInstr::ADD: 
                    execAdd(  instr);
                    PC = PC + 4;
                    break;
                case FuncInstr::ADDU:
                    execAddu( instr);
                    PC = PC + 4;
                    break;
                case FuncInstr::SUB:
                    execSub(  instr);
                    PC = PC + 4;
                    break;
                case FuncInstr::SUBU:
                    execSubu( instr);
                    PC = PC + 4;
                    break;
                case FuncInstr::SLL:
                    execSll(  instr);
                    PC = PC + 4;
                    break;
                case FuncInstr::SRL:
                    execSrl(  instr);
                    PC = PC + 4;
                    break;
                case FuncInstr::JR:
                    execJr(   instr);
                    break;
                default: assert( 0);
            }
            break;
        case FuncInstr::J_TYPE:
        case FuncInstr::I_TYPE:
            switch( instr.getOpcode())
            {
                case FuncInstr::ADDI:
                    execAddi( instr);
                    PC = PC + 4;
                    break;
                case FuncInstr::ADDIU:
                    execAddiu(instr);
                    PC = PC + 4;
                    break;
                case FuncInstr::BEQ:
                    execBeq(  instr);
                    break; 
                case FuncInstr::BNE:
                    execBne(  instr);
                    break;
                case FuncInstr::JUMP:
                    execJump( instr);
                    break;
                case FuncInstr::SW:
                    execSw(   instr);
                    PC = PC + 4;
                    break;
                case FuncInstr::SUBU/*its funct compared to lw opcode*/:
                    execLw(   instr);
                    PC = PC + 4;
                    break;
                case FuncInstr::LUI:
                    execLui(  instr);
                    PC = PC + 4;
                    break;
                default: assert( 0);
            }
            break;
        default: assert( 0);
    }
}
void FuncSim::setDumpState( bool state) const
{
    dump_state = state;
}

string FuncSim::dump() const
{ 
    return for_dump.str(); 
}

void FuncSim::simulRun()
{
    cout << hex;
    cout.setf( cout.showbase);
    cout << "Simulation runs:" << endl;
    FuncInstr tmp( 0);
    while ( tmp.getTarget() != 2 && tmp.getDest() != 2) //2 - the number of register $v0
    {
        tmp = decodeInstr( mem_file->read( PC));
        execInstr( tmp); 
        if ( dump_state)
        {
            cout << "\t" << dump() << endl;
        }
        for_dump.str("");
        ++this->instr_count;
    }
    cout << "The result is " << dec << reg_data[ 2] << endl
         << "Total number of instructions: " << this->instr_count << endl;
}

