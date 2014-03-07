/** file ~/func_sim/func_instr.h
 * It is header file with defenition of 
 * FuncInstr CLASS.
 * This class useful for:
 *      - converting opcode to instruction
 *      - presenting it to ridiable string-format
 *      - printing disassembling variant
 *
 *
 *     now implemented instructions:
 *         add, addu, sub, subu, addi, addiu, sll,
 *         srl, beq, bne, j, jr, nop, moov(1), clear
 */

#ifndef FUNC_INSTR__FUNC_INSTR_H
#define FUNC_INSTR__FUNC_INSTR_H

#include "../../common/types.h"
//Generic C++
#include <iostream>

//Generic C
#include "assert.h"
        union Bytes
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

class FuncInstr
{
    public:
        FuncInstr( uint32 bytes);
        //public function, used to gat a string with disas op
        std::string Dump( std::string indent = " ") const;
    private:
        // service functions for simpler opcodes parsing
        int initFormat( uint32 bytes);
        int parseR( );
        int parseI( );
        int parseJ( );
        
        std::string textAss;
        Bytes bytes;
};


// this operator overload std ostream stream to correctly work.
std::ostream& operator<< ( std::ostream& out, const FuncInstr& instr);



#endif // FUNC_INSTR__FUNC_INSTR_H

