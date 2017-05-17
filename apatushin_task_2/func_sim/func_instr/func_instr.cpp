#include <funct_instr.h>
#include <types.h>
#include <string>


const number_of_instr = 12;

void FunctInstr:: set_type(){
   opcode = word >> 26;
   for ( int i = 0; i < number_of_instr; i++){
                if ( ISA[i].opcode == opcode ){
                   func_type = ISA[i].func_type;
                   return;
                        }
   }
        std :: cerr << "Error : doesn't exist this instruction" << std::endl;
}

void FuncInstr :: instr_convert(){
   Convert conver_t
   conver_t.word = word;
   switch (func_type){
            case R_type:
            funct = conver_t.asR.funct;
            source = conver_t.asR.s;
            destinition = conver_t.asR.d;
            target = conver_t.asR.t;
            shamt = conver_t.asR.shamt;
            break;

        case I_type:

             funct = 0x00;
             target = conver_t.asI.t;
             source = conver_t.asI.s;
             immed = conver_t.asI.imm;
             break;
 	case J_type:
             addr = conver_t.asJ.addr;
             funct = 0x00;
             break;
        }
}

FunctInstr :: InstrInfo FuncInstr :: ISA[] = {
    { "add",   Instr_add,   Print_DST,    R_type, 0 , 20}
    { "addu",  Instr_addu,  Print_DST,   R_type, 0 , 21 }
    { "sub" ,  Instr_sub,   Print_DST,   R_type, 0 , 22 }
    { "subu",  Instr_subu,  Print_DST,   R_type, 0 , 23 }
    { "addi",  Instr_addi,  Print TSC,   I_type, 8 , 0  }
    { "addiu", Instr_addiu, Print_TSC,   I_type, 9 , 0  }
    { "sll",   Instr_sll,   Print_DTC,   R_type, 0 , 0  }
    { "srl",   Instr_srl,   Print_DTC,   R_type, 0 , 2  }
    { "beq",   Instr_beq,   Print_STC,   I_type, 4 , 0  }
    { "bne",   Instr_bne,   Print_STC,   I_type, 5 , 0  }
    { "j",     Instr_j,     Print_C,     J_type, 2 , 0  }
    { "jr",    Instr_jr,    Print_S,     R_type, 0 , 8  }
};

void FunctInstr :: define_all_fields(){

    for ( int i = 0; i < number_of_instr; i++){
        if ( ISA[i].opcode == opcode){

            name = ISA[i].name;
            pt = ISA[i].pt;
            instr = ISA[i].instr;
            return;
        }
   }
std:: cerr << "Eror doesn't exit this instruction"<< std :: endl;
}

const std :: Reg_name[] { "zero", "at", "v0", "v1", "a0", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"};

std :: string FunctInstr :: Dump( std :: string indent) const
{
    std::ostringstream oss;
    oss << indent;
    switch ( pt )
    {
        case P_NDST:
           oss << name << " " << Reg_name[destination] << ", " << Reg_name[source] << ", " << Reg_name[target];
        break;
  case P_NTSC:
            oss << name << " " << Reg_name[target] << ", " << Reg_name[source] <<", 0x" << std::hex << immed;
        break;

        case P_NDTC:
            oss << name << " " << Reg_name[destination] << ", " << Reg_name[target] << ", " << ", 0x" << std::hex shamt;
        break;

        case P_NSTC:
            oss << name << " " << Reg_name[source] << ", " << Reg_name[target] << ", 0x" << std:hex immed;
        break;

        case P_NC:
            oss << name << " 0x "<< std:hex <<addr;
        break;
        case P_NS:
            oss << name << " " << Reg_name[source];
        break;
        }
}
std ostream & operator << (std :: ostream & out, const FunctInstr & instr)
{
    return out << instr.Dump("");
}

FunctInstr :: FunctInstr( uint32 bytes)
{
    word = bytes;
    set_type();
    instr_convert();
    define_all_fields();
}







                                                                            
