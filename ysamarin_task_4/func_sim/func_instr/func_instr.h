/**
 * func_instr.h 
 * @author Yury Samarin <yuri.a.samarin@gmail.com>
 * Copyright 2012 uArchSim iLab project
 **/

// protection from multi-include
#ifndef FUNC_INSTR__FUNC_INSTR_H
#define FUNC_INSTR__FUNC_INSTR_H

// Generic C++
#include <iostream>
#include <string>

// uArchSim modules
#include <types.h>

using namespace std;

class FuncInstr
{
public:
    //Opcode/funct of instructions.
    enum NameInstr
    {
        ADDI  = 0x8,
        ADDIU = 0x9,
        BEQ   = 0x4,
        BNE   = 0x5,
        JUMP  = 0x2,
        ADD   = 0x20,
        ADDU  = 0x21,
        SUB   = 0x22,
        SUBU  = 0x23,
        SLL   = 0x0,
        SRL   = 0x2,
        JR    = 0x8,
      //LW    = 0x23 compared to SUBU, wouldn't be compiled
        SW    = 0x2B,
        LUI   = 0xF
    };

    //possible types of instructions.
    enum TypeInstr
    {
        R_TYPE,
        I_TYPE,
        J_TYPE,
    };

    //convert number into different types of instructions.
    union Converter 
    {
        uint32 asBytes;
        struct
        {
            uint32 funct: 6;
            uint32 shamt: 5;
            uint32 dest: 5;
            uint32 target: 5;
            uint32 source: 5;
            uint32 opcode: 6;
        } asR;
        struct 
        {
            uint32 immed: 16;
            uint32 target: 5;
            uint32 source: 5;
            uint32 opcode: 6;
        } asI;
        struct 
        {
            uint32 jump: 26;
            uint32 opcode: 6;
        } asJ;
    };
    enum PrintOrder
    {
        DST_ORDER,
        TS_ORDER,
        DT_ORDER,
        ST_ORDER,
        S_ORDER,
        T_ORDER,
        NO_REG
    };
    //contain information about instruction
    struct InstrInfo
    {
        TypeInstr type;
        string name;
        uint32 opcode;
        uint32 funct;
        PrintOrder reg_order;
    };
    static const short MAX_NUM_OF_INSTR; //max quantity of the instructions that we can parse.
    static const short MAX_NUM_OF_REG = 32; //max quantity of MIPS registers.
    static const short NULL_VAL = 0x0; 

    //stores list of all possible instructions in our program
    static const InstrInfo ISA[ ];
    //stores names of all MIPS registers 
    static const string REGNAME[ MAX_NUM_OF_REG];

    explicit FuncInstr( uint32 bytes);
    string Dump( string indent = " ") const;
    int setType();
    void setNameForRtype(); //set name, opcode and funct in R type instr.
    void setNameForIandJtypes(); //-|-|- in I and J type instructions
    void setSource();
    void setTarget();
    void setDest();
    void setShamt();
    void setImmed();
    void setJump();
    //following getters are needed in func_sim program
    inline uint32 getSource() const { return source; }
    inline uint32 getTarget() const { return target; }
    inline uint32 getDest()   const { return dest;   }
    inline uint32 getShamt()  const { return shamt;  }
    inline TypeInstr getType()const { return type;   }
    inline uint32 getImmed()  const { return immed;  }
    inline uint32 getJump()   const { return jump;   }
    inline uint32 getOpcode() const { return opcode; }
    inline uint32 getFunct()  const { return funct;  }
    bool is_nop() const;
    bool is_move() const;
    bool is_clear() const;

private:
    struct TurnPseudo
    {
        bool nop;
        bool move;
        bool clear;
    };

    static TurnPseudo pseudo_instr;
    Converter convert; //stores bytes and makes conversion
    TypeInstr type;
    string name;
    uint32 opcode: 6;
    uint32 funct: 6;
    string source_name;
    string target_name;
    string dest_name;
    uint32 source: 5;
    uint32 target: 5;
    uint32 dest: 5;
    uint32 shamt: 5;
    sint32 immed: 16;
    uint32 jump: 26;
    PrintOrder reg_order;

    //You can't create object with this default constructor
    FuncInstr();
};

ostream& operator << ( ostream& out, const FuncInstr& instr);

#endif // #ifndef FUNC_INSTR__FUNC_INSTR_H

