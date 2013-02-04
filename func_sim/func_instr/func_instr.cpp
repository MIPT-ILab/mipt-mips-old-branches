/**
 * func_instr.cpp - the module implementing
 * instruction decoder and disassembler.
 * 
 * 
 * 
 */

// Generic C
#include <cassert>
#include <cstring>
#include <cstdlib>

//Generic C++
#include <sstream>
#include <string>
#include <iostream>

//uArchSim modules
#include <func_instr.h>

//const for gettin' codes
#define opcode_const 4227858432 // 0-5 bits.
#define rs_const 65011712 // 6-10 bits.
#define rt_const 2031616 // 11 - 15 bits.
#define rd_const 63488 //16-20 bits.
#define sh_const 1984 //21-25 bits.
#define imm_const 65535 // last 16 bits.
#define addr_const 134217727 //last 26 bits = 2^27-1.
//funct_const = 2^6-1 = 63.

FuncInstr::FuncInstr ( uint32 bytes)
{
    get_form_type ( bytes);    
};

uint32 FuncInstr::get_opcode ( uint32 bytes)
{
    return ( bytes >> 26);
};

uint32 FuncInstr::get_funct ( uint32 bytes)
{
    return ( bytes & 63); 
};

uint32 FuncInstr::get_rs ( uint32 bytes)
{
    return (( bytes & rs_const) >> 20);
};

uint32 FuncInstr::get_rt ( uint32 bytes)
{
    return (( bytes & rt_const) >> 15);
};

uint32 FuncInstr::get_rd ( uint32 bytes)
{
    return (( bytes & rd_const) >> 10);
};

uint32 FuncInstr::get_shmt ( uint32 bytes)
{
    return (( bytes & sh_const) >> 5);
}

uint32 FuncInstr::get_imm ( uint32 bytes)
{
    return (( bytes & imm_const));
};

uint32 FuncInstr::get_addr ( uint32 bytes)
{
    return ( bytes & addr_const);
};

void FuncInstr::get_form_type ( uint32 bytes)
{
	opcode = get_opcode ( bytes);
	switch ( opcode)
	{
	    case 0: 
	    fType = R;
            get_R_name ( bytes);
	    break;   //AL and shift
        case 2: 
            fType = J;
            addr = get_addr ( bytes);
            InstrName = j;  // jump
            break;  
        case 4:
            fType = I;
            reg = get_rs ( bytes);
            reg1 = get_reg ( reg);
            reg = get_rd ( bytes);
            reg2 = get_reg( reg);
            imm = get_imm ( bytes);
            InstrName = beq; // branch on equal
            break;
        case 5:
            fType = I;
            reg = get_rs ( bytes);
            reg1 = get_reg ( reg);
            reg = get_rd ( bytes);
            reg2 = get_reg( reg);
            imm = get_imm ( bytes);            
            InstrName = bne;  // branch on not equal
            break;
        case 8:
            fType = I;
            reg = get_rs ( bytes);
            reg1 = get_reg ( reg);
            reg = get_rd ( bytes);
            reg2 = get_reg( reg);
            imm = get_imm ( bytes); 
            InstrName = addi; // add immediate
            break;
        case 9:
            fType = I;
            reg = get_rs ( bytes);
            reg1 = get_reg ( reg);
            reg = get_rd ( bytes);
            reg2 = get_reg( reg);
            imm = get_imm ( bytes);  
            if ( imm == 0) InstrName = move;           
            else InstrName = addiu;  // add imm. unsigned
            break;
        default:
            cout << "This instruction isn't supported yet" << endl;
            exit ( -1);
            break;
	}
};

//gettin' operands also.
void FuncInstr::get_R_name ( uint32 bytes)
{
    funct = get_funct ( bytes);
    switch ( funct)
    {
        case 0: 
            reg = get_rd ( bytes);
            reg1 = get_reg ( reg);
            reg = get_rs ( bytes);
            reg2 = get_reg ( reg);
            shmt = get_shmt ( bytes);
            if (( shmt == 0) && ( reg1 == $zero) && ( reg2 == $zero))
            {
                InstrName = nop;
                reg1 = empty;
                reg2 = empty;
            }
            else InstrName = sll; //shift left logical;
            reg3 = empty;
            break;
        case 2:
            InstrName = srl;  // shift right logical;
            reg = get_rd ( bytes);
            reg1 = get_reg ( reg);
            reg = get_rs ( bytes);
            reg2 = get_reg ( reg);
            shmt = get_shmt ( bytes);
            reg3 = empty;
            break;
        case 8:
            InstrName = jr;  // jump to register
            reg = get_rs ( bytes);
            reg1 = get_reg ( reg);
            break;
        case 20:
            InstrName = add;  // no comments.
            reg = get_rd ( bytes);
            reg1 = get_reg ( reg);
            reg = get_rs ( bytes);
            reg2 = get_reg ( reg);
            reg = get_rt ( bytes);
            reg3 = get_reg ( bytes);
            break;
        case 21:
            reg = get_rd ( bytes);
            reg1 = get_reg ( reg);
            reg = get_rs ( bytes);
            reg2 = get_reg ( reg);
            reg = get_rt ( bytes);
            reg3 = get_reg ( bytes);
            if (( reg1 == $zero) && ( reg2 == $zero))
	    {
		InstrName = clear;
                reg1 = reg3;
                reg2 = empty;
                reg3 = empty;
            }
            else InstrName = addu; // add unsigned;
            break;
        case 22:
            InstrName = sub; //substract.
            reg = get_rd ( bytes);
            reg1 = get_reg ( reg);
            reg = get_rs ( bytes);
            reg2 = get_reg ( reg);
            reg = get_rt ( bytes);
            reg3 = get_reg ( bytes);
            break;
        case 23:
            InstrName = subu; // sub unsigned.  
            reg = get_rd ( bytes);
            reg1 = get_reg ( reg);
            reg = get_rs ( bytes);
            reg2 = get_reg ( reg);
            reg = get_rt ( bytes);
            reg3 = get_reg ( bytes);
            break;
        default:
            cout << "Undef.functor. This instruction isn't supported yet" << endl;
            exit ( -1);
            break;     
    }
};

reg_addr FuncInstr::get_reg ( uint32 reg)
{
    switch ( reg)
    {
        case 0: return $zero; break;
        case 1: return $at; break;
        case 2: return $v0; break;
        case 3: return $v1; break;
        case 4: return $a0; break;
        case 5: return $a1; break;
        case 6: return $a2; break;
        case 7: return $a3; break;
        case 8: return $t0; break;
        case 9: return $t1; break;
        case 10: return $t2; break;
        case 11: return $t3; break;
        case 12: return $t4; break;
        case 13: return $t5; break;
        case 14: return $t6; break;
        case 15: return $t7; break;
        case 16: return $s0; break;
        case 17: return $s1; break;
        case 18: return $s2; break;
        case 19: return $s3; break;
        case 20: return $s4; break;
        case 21: return $s5; break;
        case 22: return $s6; break;
        case 23: return $s7; break;
        case 24: return $t8; break;
        case 25: return $t9; break;
        case 26: return $k0; break;
        case 27: return $k1; break;
        case 28: return $gp; break;
        case 29: return $sp; break;
        case 30: return $fp; break;
        case 31: return $ra; break;
        default: break;   // can be no exceptions because
                          // reg can't be >= 32
    }
};


string FuncInstr::Dump ( string indent) const
{
    ostringstream oss;
    oss << indent << this->InstrName << indent;
    if ( this->fType == R)
    {
        if ( this->reg1 != empty) oss << this->reg1 << indent;
        if ( this->reg2 != empty) oss << this->reg2 << indent;
        if ( this->reg3 != empty) oss << this->reg3 << indent;
        else if ( this->shmt) oss << this->shmt;
        //return oss.str();
    }
    else if ( this->fType == I)
    {
        oss << this->reg1 << indent << this->reg2 << indent;
        if ( this->imm) oss << this->imm;
        //return oss.str();
    }
    else if ( this->fType == J)
    {
        oss << this->addr;
        //return oss.str();
    };
    return oss.str();
};

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr)
{
    out << instr.Dump(string indent = " ");
    return out.str();
};
