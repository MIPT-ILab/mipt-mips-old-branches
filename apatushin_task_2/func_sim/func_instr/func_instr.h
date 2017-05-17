#ifdef FUNC_INSTR__FUNC_INSTR_H
#define FUNC_INSTR__FUNC_INSTR_H

#include <types.h>

class FuncInstr
{
    enum Type {
        R_type,
        J_type,
        I_type
        };
    enum Instruction { Inst_add, Inst_addu, Inst_sub, Inst_subu, Inst_addi, Inst_addiu, Inst_sll, Inst_srl, Inst_beq, Inst_bne, Inst_j,Inst_jr};
   
    enum Print { P_NDST, P_NTSC, P_NDTC, P_NSTC, P_NC, P_NS};

    Type func_type;
    std :: string name;
    Instruction instr;
    Print pt;
    uint32 word;
    uint32 opcode;
    uint32 funct;
    uint32 addr;
    uint32 shamt;
    uint32 immed;
    uint32 source;
    uint32 target;
    uint32 destination;

     struct Instr_Info{
        std :: string name;
        Instruction inst;
        Print pt;
        Type func_type;
        uint32 opcode;
        uint32 funct;
        }

    union Convert{

        struct {

                uint32 funct : 6;
                uint32 shamt : 5;
                uint32 d : 5;
                uint32 t : 5;
                uint32 s : 5;
                uint32 opcode : 6;
                } asR;
       struct {

                uint32 imm : 16;
                uint32 t : 5;
                uint32 s : 5;
                uint32 opcode : 6;
                } asI;

       struct {

                uint32 addr : 26;
                uint32 opcode : 6;
                } asJ;
          uint32 word;
        };

   void set_type();
   void instr_convert();
   void define_all_field();
   public :
                static InstrInfo ISA[];
                FuncInstr( uint32 bytes );
        std:: string Dump ( std :: string indent = " ") const;
};

#endif





