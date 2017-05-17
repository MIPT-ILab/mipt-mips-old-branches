/**
 * func_instr.h - Header of FuncInstr class
 * @author Andrey Kochetygov <andrey.kochetygov@gmail.com>
 * Copyrignt 2012 uArchSim iLab project
 */

#ifndef FUNC_INSTR__FUNC_INSTR_H
#define FUNC_INSTR__FUNC_INSTR_H

// Generic C++
#include <iostream>
#include <string>

// uArchSim modules
#include <types.h>


class FuncInstr
{
    public:
        enum format_type { RT, IT, JT };
        enum register_name { $zero, $at, $t0, $t1, $t2, $t3, $t4, $t5, $t6, $t7, $t8, $t9 };
        enum type_of_instruction { add1, addu1, sub1, subu1, addi1, addiu1, sll1, srl1, beq1, bne1, j1, jr1, move1, clear1 };
        format_type ift;
        std::string name;
        static const char * allregisters[];
        int reg1;
        int reg2;
        int reg3;
        uint32 code;
        uint32 funct;
        uint32 isfound;//check if do not no this instruction yet
        uint32 shamt;
        uint32 printform;//how should we print according to the type
        register_name dst;
        register_name trg;
        register_name src;
        uint32 opcode;
        type_of_instruction ecurrname;
        std::string currname;
        uint32 constant;
        uint32 jmp;
        struct command
        {
            std::string name;
            type_of_instruction instr_name;
            format_type  type;
            uint32  opcodecomm;
            uint32 funct;
            uint32 printform;
        };
        static command allcommands[];
        union divide
        {
            struct
            {
                uint32 funct: 6;
                uint32 shamt: 5;
                uint32 dst: 5;
                uint32 trg: 5;
                uint32 src: 5;
                uint32 opcode: 6;
            } RST;
            struct
            {
                uint32 constant: 16;
                uint32 trg: 5;
                uint32 src: 5;
                uint32 opcode: 6;
            } IST;
            struct
            {
                uint32 jmp: 26;
                uint32 opcode: 6;
            } JST;
        };
        divide *curr;
        void RTdivider();
        void JTdivider();
        void ITdivider();
        void ispseudo();//check if it is a pseudo instr
        uint32 pseudo;
        FuncInstr( uint32 bytes);
        std::string Dump( std::string indent = " ") const;


};

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr);

#endif


