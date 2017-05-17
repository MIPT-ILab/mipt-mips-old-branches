/** file ~/func_sim/func_instr.h
 * It is header file with defenition of 
 * FuncInstr CLASS.
 * This class useful for:
 *      - converting opcode to instruction
 *      - presenting it to ridiable string-format
 *      - printing disassembling variant
 *
 *     now implemented instructions:
 *          j, jal, beq, bne, addi, addiu, lui, 
 *          lw, sw, sll, srl, sllv, srlv, arav,
 *          jr, syscall, mflo, multu, add, addu,
 *          sub, suby, and, or, xor, nor, slt,
 *          nop, move, clear (30).
 */

#ifndef FUNC_INSTR__FUNC_INSTR_H
#define FUNC_INSTR__FUNC_INSTR_H

#include "../../common/types.h"
//Generic C++
#include <iostream>
#include <sstream>

//Generic C
#include "assert.h"

class FuncInstr
{
    public:
        FuncInstr( uint32 bytes);
        //public function, used to gat a string with disas op
        std::string Dump( std::string indent = " ") const;
    private:
        // service functions for simpler opcodes parsing
        int initFormat( uint32 bytes);
        std::string textAss;
    public:    
        union Bytes // used to parsing bits from op. code.
        {
            struct
            {
                unsigned imm:16;
                unsigned t:5;
                unsigned s:5;
                unsigned opcode:6;
            } asI;
            struct
            {
                unsigned funct:6;
                unsigned shamt:5;
                unsigned d:5;
                unsigned t:5;
                unsigned s:5;
                unsigned opcode:6;
            } asR;
            struct
            {
                unsigned address:26;
                unsigned opcode:6;                
            } asJ;
            uint32 raw;
        };
    private:
        Bytes bytes;
};

// this operator overload std ostream stream to correctly work.
std::ostream& operator<< ( std::ostream& out, const FuncInstr& instr);

#endif // FUNC_INSTR__FUNC_INSTR_H

