/**
 * func_instr.h - Header of module implementing instruction 
 * decoder and disassembler.
 */


//protection from multi-include
#ifndef FUNC_INSTR__FUNC_INSTR_H
#define FUNC_INSTR__FUNC_INSTR_H

// Generic C++
#include <string>

//uArchSim modules
#include <types.h>

using namespace std;

class FuncInstr
{
    //uint32 instr_bytes;
    uint32 opcode, funct;
    //string InstrName;
    uint32 reg, shmt, imm, addr;
    uint32 get_opcode ( uint32 bytes);
    
    void get_form_type( uint32 bytes);
    void get_R_name( uint32 bytes);
    
    uint32 get_funct ( uint32 bytes);
    uint32 get_rs ( uint32 bytes);
    uint32 get_rt ( uint32 bytes);
    uint32 get_rd ( uint32 bytes);
    uint32 get_imm ( uint32 bytes);
    uint32 get_shmt ( uint32 bytes);
    uint32 get_addr( uint32 bytes);
    
    enum form_type { R, I, J} fType;
    enum reg_addr { $zero, $at, $v0, $v1, $a0, $a1, $a2, $a3,
    $t0, $t1, $t2, $t3, $t4, $t5, $t6, $t7, $s0, $s1, $s2, $s3,
    $s4, $s5, $s6, $s7, $k0, $k1, $gp, $sp, $fp, $ra, empty} reg1, reg2, reg3;
    reg_addr get_reg( uint32 bytes);
    enum instr_type { add, addu, sub, subu, addi, addiu, sll, srl,
    beq, bne, j, jr, move, clear, nop} InstrName;
//  enum reg_addr
//  enum instr_type
    public:
        FuncInstr ( uint32 bytes);
        
	std::string Dump( std::string indent = " ") const;
};

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr);
#endif //#ifndef.
