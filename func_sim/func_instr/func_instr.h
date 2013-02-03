/**
 * func_instr.h - Header of FuncInstr class
 * @author Elena Limonova <elena.e.limonova@gmail.com>
 * Copyrignt 2012 uArchSim iLab project
 */

#ifndef FUNC_INSTR__FUNC_INSTR_H
#define FUNC_INSTR__FUNC_INSTR_H

#define NUM_OF_INSTR 12

#include <string>

#include <types.h>

class FuncInstr
{
    public:
        enum FormatType { JType, IType, RType};
        enum Instruction { add, addu, sub, subu, addi, addiu, sll, srl, beq, bne, 
                           j, jr, move, clear, nop};
        enum PrintType { dst, tsc, dtc, stc, c, s, ts, t, none}; //Output type
        enum Register { zero, at, v0, v1, a0, a1, a2, a3, t0, t1, t2, t3, t4, t5, 
                        t6, t7, s0, s1, s2, s3, s4, s5, s6, s7, t8, t9, k0, k1, 
                        gp, sp, fp, ra};

        FormatType type; 
        PrintType pType;
        std::string name; 
        Instruction instr;
        Register source;
        Register target;
        Register dest;
        uint32 shamt; //constant value for shifts
        uint32 iConstVal; //immediate constant value for ALU operations
        uint32 address; //address for jumps and procedure calls
        void convertInstr( FormatType type, uint32 instr);
        FormatType getType( uint32 opcode);
        uint32 getOpcode( uint32 bytes);
        uint32 getFunct( uint32 bytes);
        class InstrInfo
        {
            public: 
                std::string name;
                Instruction instr;
                FormatType type;
                PrintType pType;
                uint32 opcode;
                uint32 funct;
                InstrInfo();
                InstrInfo( std::string instrName, Instruction instrInstr, PrintType printType, 
                FormatType instrType, uint32 instrOpcode, uint32 instrFunct);
        };
        static InstrInfo ISA[ NUM_OF_INSTR];
        static const char * myRegNames[ 32];
        union Convert //decode instruction for each type
        {
	        struct
	        {
		        uint32 funct : 6;
	    	    uint32 shamt : 5;
	    	    uint32 d : 5;
	    	    uint32 t : 5;
	    	    uint32 s : 5;
	    	    uint32 opcode : 6;
    	    } asR;
    	    struct
    	    {
	    	    uint32 iConst : 16;
	    	    uint32 t : 5;
	    	    uint32 s : 5;
	    	    uint32 opcode : 6;
    	    } asI;
    	    struct 
    	    {
	    	    uint32 addr : 26;
	    	    uint32 opcode : 6;
    	    } asJ;
        };

        FuncInstr( uint32 bytes);
        std::string Dump( std::string indent = " ") const;
};

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr);

#endif
