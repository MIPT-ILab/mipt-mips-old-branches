/**
 * func_instr.cpp - Implementation of MIPS Disassembler
 * @author Semyon Abramov <semyon.abramov.mipt@gmail.com>
 * 2014 MIPT-MIPS iLab project
 */

// uArchSim modules
#include"func_instr.h"

using namespace std;

/*  MIPS ISA Table */
const FuncInstr::ISAEntry FuncInstr::isaTable[ ] =
{
    // name   opcode    func      format              type               counter
    { "add",   0x0,     0x20, FuncInstr::FORMAT_R,    FuncInstr::ADD,     0x0    },
    { "addu",  0x0,     0x21, FuncInstr::FORMAT_R,    FuncInstr::ADDU,    0x1    },
    { "sub",   0x0,     0x22, FuncInstr::FORMAT_R,    FuncInstr::SUB,     0x2    },
    { "subu",  0x0,     0x23, FuncInstr::FORMAT_R,    FuncInstr::SUBU,    0x3    },
    { "addi",  0x8,     0x0,  FuncInstr::FORMAT_I,    FuncInstr::ADDI,    0x4    },
    { "addiu", 0x9,     0x0,  FuncInstr::FORMAT_I,    FuncInstr::ADDIU,   0x5    },
    { "sll",   0x0,     0x0,  FuncInstr::FORMAT_R,    FuncInstr::SLL,     0x6    },
    { "srl",   0x0,     0x2,  FuncInstr::FORMAT_R,    FuncInstr::SRL,     0x7    },
    { "addu",  0x0,     0x21, FuncInstr::FORMAT_R,    FuncInstr::ADD,     0x8    },
    { "beq",   0x4,     0x0,  FuncInstr::FORMAT_I,    FuncInstr::BEQ,     0x9    },
    { "bne",   0x5,     0x0,  FuncInstr::FORMAT_I,    FuncInstr::BNE,     0xA    },
    { "j",     0x2,     0x0,  FuncInstr::FORMAT_J,    FuncInstr::J,       0xB    },
    { "jr",    0x0,     0x8,  FuncInstr::FORMAT_R,    FuncInstr::JR,      0xC    },
    { "undef", 0,       0,    FuncInstr::UNDEF_FORMAT,FuncInstr::UNDEF,   0xFF   }
};

/* Register Table for MIPS architecture */
const char* FuncInstr::regTable[ ] =
{
    "$zero",                                          // constant 0
    "$at",                                            // assembler temporary
    "$v0","$v1",                                      // function result registers
    "$a0","$a1","$a2","$a3",                          // function argument registers that hold first four words of int type arguments
    "$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7",  // temporaries
    "$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7",  // saved registers to use freely
    "$t8","$t9",                                      // temporaries
    "$k0","$k1",                                      // reserved for use by the operating system kernel and for exception return
    "$gp",                                            // global pointer
    "$sp",                                            // stack pointer
    "$fp",
    "$ra"                                             // return address register
};

/* This function initializes format of MIPS instruction */
void FuncInstr::initFormat( uint32 bytes)
{
    for( int i = 0; i < isaTable_size; ++i)
    {
        if (( this -> bytes.asR.opcode) == this -> isaTable[ i].opcode)
        {
            format = this -> isaTable[ i].format;
            break;
        }
        else
        {
            format = UNDEF_FORMAT;
        }


    }
    if( format == UNDEF_FORMAT)
	{
		cerr << "ERROR.*" << endl;   // return error if instruction's format isn't defined
		exit(EXIT_FAILURE);
	}
}


/* Constructor of FuncInstr class */
FuncInstr::FuncInstr( uint32 bytes)
{
    this -> bytes.raw = bytes;
    this -> initFormat( bytes);
    switch ( this -> format)
    {
        case FORMAT_R:
            this -> parseR( bytes);
            break;
        case FORMAT_I:
            this -> parseI( bytes);
            break;
        case FORMAT_J:
            this -> parseJ( bytes);
            break;
        default:
            assert( 0);
    }
}

/* This function parses input 4 bytes according to R format */
void FuncInstr::parseR( uint32 bytes)
{
    for( int i = 0; i < isaTable_size; ++i)
    {
        if ( ( this -> bytes.asR.opcode == isaTable[ i].opcode)
            && ( this -> bytes.asR.funct == isaTable[ i].func))
        {
            oper_name = isaTable[ i].name;
            oper_type = isaTable[ i].type;
            break;
        }
        else
        {
            oper_type = UNDEF;
        }

    }
	if ( oper_type == UNDEF)                                                    // check if operation type is not defined
	{
		#if ASSERT
            cout << "ILLEGAL INSTRUCTION:  " << endl;
            cout << "THERE IS NOT SUCH INSTRUCTION IN THE MIPS ISA !!!" << endl;
            assert( 0);
		#endif
		cout << "ILLEGAL INSTRUCTION:  " << endl;                               // return error if operation type is not defined
		cout << "THERE IS NO SUCH INSTRUCTION IN THE MIPS ISA !!!" << endl;
		exit(EXIT_FAILURE);
    }
    /* parsing according to operation type */
    switch( oper_type)
    {
        case SLL:
            Reg_t = regTable[ this -> bytes.asR.t];
            Reg_d = regTable[ this -> bytes.asR.d];
            shamt = this -> bytes.asR.shamt;
            break;
        case SRL:
            Reg_t = regTable[ this -> bytes.asR.t];
            Reg_d = regTable[ this -> bytes.asR.d];
            shamt = this -> bytes.asR.shamt;
            break;
        case JR:
            Reg_s = regTable[ this -> bytes.asR.s];
            break;
       	case UNDEF:
            assert(0);
        default:
            Reg_t = regTable[ this -> bytes.asR.t];
            Reg_s = regTable[ this -> bytes.asR.s];
            Reg_d = regTable[ this -> bytes.asR.d];
            break;
    }
}

/*  This function parses input 4 bytes according to I format */
void FuncInstr::parseI( uint32 bytes)
{
    for( int i = 0; i < isaTable_size; ++i)
    {
        if ( this -> bytes.asI.opcode == isaTable[ i].opcode)
        {
            oper_name = isaTable[ i].name;
            oper_type = isaTable[ i].type;
            break;
        }
        else
        {
            oper_type = UNDEF;
        }

    }
    if ( oper_type == UNDEF)                                                   //  check if operation type is not defined
	{
		cout << "ILLEGAL INSTRUCTION:  " << endl;                               // return error if operation type is not defined
		cout << "THERE IS NO SUCH INSTRUCTION IN THE MIPS ISA !!!" << endl;
		exit(EXIT_FAILURE);
	}
    /* if operation type is defined it gets registers names and immediate value */
    Reg_t = regTable[ this -> bytes.asI.t];
    Reg_s = regTable[ this -> bytes.asI.s];
    immediate = this -> bytes.asI.imm;
}

/* This function parses input 4 bytes according to J format */
void FuncInstr::parseJ( uint32 bytes)
{
     for( int i = 0; i < isaTable_size; ++i)
     {
         if ( this -> bytes.asI.opcode == isaTable[ i].opcode)
         {
             oper_name = isaTable[ i].name;
             oper_type = isaTable[ i].type;
             break;
         }
         else
         {
             oper_type = UNDEF;
		 }
     }
     if ( oper_type == UNDEF)                                                   //  check if operation type is not defined
     {
        cout << "ILLEGAL INSTRUCTION:  " << endl;                               // return error if operation type is not defined
        cout << "THERE IS NO SUCH INSTRUCTION IN THE MIPS ISA !!!" << endl;
		exit(EXIT_FAILURE);
	 }
     jump_addr = this -> bytes.asJ.addr;                                        // if operation type is defined it gets jump address
}

/* This method  returns disassembly of instructions */
string FuncInstr::Dump( string indent) const
{
    std::ostringstream oss;
    /* disassembling according to operation format */
    switch( this -> format)
    {
        case FORMAT_R:
            /* disassembling of R format operation according to operation type */
            switch( oper_type)
            {
                case SRL:
                    oss << oper_name << " " << Reg_d << ", " << Reg_t << ", " << "0x" << hex << shamt;
                    break;
                case SLL:
                    oss << oper_name << " " << Reg_d << ", " << Reg_t << ", " << "0x" << hex << shamt;
                    break;
                case JR:
                    oss << oper_name << " " << Reg_s;
                    break;
                default:
                    oss << oper_name << " " << Reg_d << ", " << Reg_s << ", " << Reg_t;
                    break;
            };
            break;
        case FORMAT_I:
            oss << oper_name << " " << Reg_t << ", " << Reg_s << ", " << "0x" << hex << immediate;
            break;
        case FORMAT_J:
            oss << oper_name << " "<< "0x" << hex << jump_addr;
            break;
        default:
            assert( 0);
     }
     return oss.str();
}

/* implementation of operator "<<" overloading for class FuncInstr */

std::ostream& operator<< ( std::ostream& out, const FuncInstr& instr)
{
     return out << instr.Dump( "");
}


