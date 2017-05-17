/**
 * func_instr.cpp
 * Implementation of the MIPS instruction decoder.
 * MIPT-MIPS-2014 Assignment 2.
 * Ladin Oleg.
 */

// Generic C
#include <assert.h>
#include <stdlib.h>

// Generic C++
#include <sstream>

// uArchSim modules
#include <func_instr.h>

const FuncInstr::ISAEntry FuncInstr::isa_table[] = // Array of instructions
{
    /*
     * group                                 name
     * code            format                opcode func fields  number
     */

    { FuncInstr::GROUP_ADD_SUBTRACT,         ( char*)"add",
      ( char*)"add",   FuncInstr::FORMAT_R,  0x0,  0x20, 0x3887, 0x0  },

    { FuncInstr::GROUP_ADD_SUBTRACT,         ( char*)"add unsigned",
      ( char*)"addu",  FuncInstr::FORMAT_R,  0x0,  0x21, 0x3887, 0x1  },

    { FuncInstr::GROUP_ADD_SUBTRACT,         ( char*)"substract",
      ( char*)"sub",   FuncInstr::FORMAT_R,  0x0,  0x22, 0x3887, 0x2  },

    { FuncInstr::GROUP_ADD_SUBTRACT,         ( char*)"substract unsigned",
      ( char*)"subu",  FuncInstr::FORMAT_R,  0x0,  0x23, 0x3887, 0x3  },

    { FuncInstr::GROUP_ADD_SUBTRACT,         ( char*)"add immediate",
      ( char*)"addi",  FuncInstr::FORMAT_I,  0x8,  0xFF, 0x7848, 0x4  },

    { FuncInstr::GROUP_ADD_SUBTRACT,         ( char*)"add immediate unsigned",
      ( char*)"addiu", FuncInstr::FORMAT_I,  0x9,  0xFF, 0x7848, 0x5  },

    { FuncInstr::GROUP_SHIFTS,               ( char*)"shift left logical",
      ( char*)"sll",   FuncInstr::FORMAT_R,  0x0,  0x0,  0x1C9C, 0x6  },

    { FuncInstr::GROUP_SHIFTS,               ( char*)"shift right logical",
      ( char*)"srl",   FuncInstr::FORMAT_R,  0x0,  0x2,  0x1C9C, 0x7  },

    { FuncInstr::GROUP_CONDITIONAL_BRANCHES, ( char*)"branch on equal",
      ( char*)"beq",   FuncInstr::FORMAT_I,  0x4,  0xFF, 0x7818, 0x8  },

    { FuncInstr::GROUP_CONDITIONAL_BRANCHES, ( char*)"branch on not equal",
      ( char*)"bne",   FuncInstr::FORMAT_I,  0x5,  0xFF, 0x7818, 0x9  },

    { FuncInstr::GROUP_UNCONDITIONAL_JUMP,   ( char*)"jump",
      ( char*)"j",     FuncInstr::FORMAT_J,  0x2,  0xFF, 0xA000, 0xA  },

    { FuncInstr::GROUP_UNCONDITIONAL_JUMP,   ( char*)"jump register",
      ( char*)"jr",    FuncInstr::FORMAT_R,  0x0,  0x8,  0x201B, 0xB  },

    { FuncInstr::NO_GROUP,                   0,
      0,               FuncInstr::NO_FORMAT, 0xFF, 0xFF, 0,      0xFF }
};
const char* FuncInstr::reg_table[] = // Array of register names
{
    ( char*)"$zero",                                            //0
    ( char*)"$at",                                              //1
    ( char*)"$v0", ( char*)"$v1",                               //2, 3
    ( char*)"$a0", ( char*)"$a1", ( char*)"$a2", ( char*)"$a3", //4 - 7
    ( char*)"$t0", ( char*)"$t1", ( char*)"$t2", ( char*)"$t3", //8 - 15
    ( char*)"$t4", ( char*)"$t5", ( char*)"$t6", ( char*)"$t7",
    ( char*)"$s0", ( char*)"$s1", ( char*)"$s2", ( char*)"$s3", //16 - 23
    ( char*)"$s4", ( char*)"$s5", ( char*)"$s6", ( char*)"$s7",
    ( char*)"$t8", ( char*)"$t9",                               //24, 25
    ( char*)"$k0", ( char*)"$k1",                               //26, 27
    ( char*)"$gp",                                              //28
    ( char*)"$sp",                                              //29
    ( char*)"$fp",                                              //30
    ( char*)"$ra"                                               //31
};

FuncInstr::FuncInstr( uint32 bytes)
{
    /** Constructor parses and saves instruction. */
    for ( int i = 0; i < 5; i++) // Clear output structure
    {
        this->dmp[ i].type = 0;
        this->dmp[ i].str = 0;
        this->dmp[ i].num = 0;
    }
    this->bytes.raw = bytes; // Save numeric representation of the instruction
    this->initFormat( bytes); // Recognize format
    /* Depending on format, parse instruction. */
    switch ( this->isa_table[ this->num].format)
    {
        case FORMAT_R:
            this->parseR( bytes);
            break;
        case FORMAT_I:
            this->parseI( bytes);
            break;
        case FORMAT_J:
            this->parseJ( bytes);
            break;
        default: // Error
            assert( 0);
    }
}

void FuncInstr::initFormat( uint32 bytes)
{
    /** Brute-force searching suitable opcode (and funct). */
    int i; // Counter
    for ( i = 0; this->isa_table[ i].number != 0xFF; i++) // While not end
    {
        if ( this->isa_table[ i].opcode == this->bytes.asR.opcode) // Compare opcodes
        {
            if ( this->isa_table[ i].func != 0xFF) // If it maybe R
            {
                if ( this->isa_table[ i].func == this->bytes.asR.func) // If it's R
                {
                    this->num = this->isa_table[ i].number; // Save index number
                    break;
                }
            } else // If it's I or J
            {
                this->num = this->isa_table[ i].number; // Save index number
                break;
            }
        }
    }
    /* If reached end member (no same instruction in the implementation). */
    if ( this->isa_table[ i].number == 0xFF)
    {
        std::cerr << "ERROR: CAN'T DECODE: " << std::hex << bytes << std::dec
                  << std::endl;
        exit( EXIT_FAILURE);
    }
    return;
}

void FuncInstr::parseR( uint32 bytes)
{
    /** Parcing R-type instruction. */
    uint8 reg; // Number of register/constant field suitable 'i' structure
    /* Instruction code - string is in the first structure. */
    this->dmp[ 0].type = 1;
    this->dmp[ 0].str = this->isa_table[ this->num].code;
    /* Set output ('dmp') fields: max 3 registers and 1 constant. */
    for ( int i = 1; i < 5; i++)
    {
        /* Read 2 bits of last byte of 'fields', convert into 'reg' number. */
        reg = ( ( ( this->isa_table[ this->num].fields) & ( 1 << ( 9 - i * 2))) >>
                ( 9 - i * 2) ) * 2 + ( ( ( this->isa_table[ this->num].fields)
                                         & ( 1 << ( 8 - i * 2))) >> ( 8 - i * 2));
        /* Check we need to print field with number 'reg' or not. */
        if ( ( ( this->isa_table[ this->num].fields) & ( 1 << ( 13 - reg))) != 0)
        {
            switch ( reg)
            {
                case 0: // rs, string
                    this->dmp[ i].type = 1;
                    this->dmp[ i].str = ( char*)this->reg_table[ this->bytes.asR.rs];
                    break;
                case 1: // rt, string
                    this->dmp[ i].type = 1;
                    this->dmp[ i].str = ( char*)this->reg_table[ this->bytes.asR.rt];
                    break;
                case 2: // rd, string
                    this->dmp[ i].type = 1;
                    this->dmp[ i].str = ( char*)this->reg_table[ this->bytes.asR.rd];
                    break;
                case 3: // sh (S), number
                    this->dmp[ i].type = 2;
                    this->dmp[ i].num = this->bytes.asR.sh;
                    break;
                default: // Error
                    assert( 0);
            }
        }
    }
    return;
}

void FuncInstr::parseI( uint32 bytes)
{
    /** Parcing I-type instruction. */
    uint8 reg; // Number of register/constant field suitable 'i' structure
    /* Instruction code - string is in the first structure. */
    this->dmp[ 0].type = 1;
    this->dmp[ 0].str = this->isa_table[ this->num].code;
    /* Set output ('dmp') fields: max 2 registers and 1 constant. */
    for ( int i = 1; i < 4; i++)
    {
        /* Read 2 bits of last byte of 'fields', convert into 'reg' number. */
        reg = ( ( ( this->isa_table[ this->num].fields) & ( 1 << ( 9 - i * 2))) >>
                ( 9 - i * 2) ) * 2 + ( ( ( this->isa_table[ this->num].fields)
                                         & ( 1 << ( 8 - i * 2))) >> ( 8 - i * 2));
        /* Check we need to print field with number 'reg' or not. */
        if ( ( ( this->isa_table[ this->num].fields) & ( 1 << ( 13 - reg))) != 0)
        {
            switch ( reg)
            {
                case 0: // rs, string
                    this->dmp[ i].type = 1;
                    this->dmp[ i].str = ( char*)this->reg_table[ this->bytes.asI.rs];
                    break;
                case 1: // rt, string
                    this->dmp[ i].type = 1;
                    this->dmp[ i].str = ( char*)this->reg_table[ this->bytes.asI.rt];
                    break;
                case 2: // imm (C), number
                    this->dmp[ i].type = 2;
                    this->dmp[ i].num = this->bytes.asI.imm;
                    break;
                default: // Error
                    assert( 0);
            }
        }
    }
    return;
}

void FuncInstr::parseJ( uint32 bytes)
{
    /** Parcing J-type instruction. */
    uint8 reg; // Number of register/constant field suitable 'i' structure
    /* Instruction code - string is in the first structure. */
    this->dmp[ 0].type = 1;
    this->dmp[ 0].str = this->isa_table[ this->num].code;
    /* Set output ('dmp') fields: max 1 constant. */
    for ( int i = 1; i < 2; i++)
    {
        /* Read 2 bits of last byte of 'fields', convert into 'reg' number. */
        reg = ( ( ( this->isa_table[ this->num].fields) & ( 1 << ( 9 - i * 2))) >>
                ( 9 - i * 2) ) * 2 + ( ( ( this->isa_table[ this->num].fields)
                                         & ( 1 << ( 8 - i * 2))) >> ( 8 - i * 2));
        /* Check we need to print field with number 'reg' or not. */
        if ( ( ( this->isa_table[ this->num].fields) & ( 1 << ( 13 - reg))) != 0)
        {
            switch ( reg)
            {
                case 0: // add (A), number
                    this->dmp[ i].type = 2;
                    this->dmp[ i].num = this->bytes.asJ.add;
                    break;
                default: // Error
                    assert( 0);
            }
        }
    }
    return;
}

std::string FuncInstr::Dump( std::string indent) const
{
    /**
     * Dump method returns prepared string:
     * <indent><code> <r/c>[, <r/c>][, <r/c>][, <r/c>]
     * there r/c - register name or constant in HEX format.
     */
    std::ostringstream oss; // Output
    oss << indent; // Indent
    /* Check each element 'dmp' we need to print it or not. */
    for ( int i = 0; i < 5; i++)
    {
        switch ( this->dmp[ i].type)
        {
            case 0: // Don't print
                i = 5; // Exit, because 'type'=="0" means end of output string
                break;
            case 1: // If it's string
                if ( i > 0) // After 'code' and later goes indent
                {
                    oss << " ";
                }
                oss << this->dmp[ i].str; // String
                /* Do we need to put comma? */
                if ( ( i > 0) && ( i < 4) && ( this->dmp[ i + 1].type != 0))
                {
                    oss << ",";
                }
                break;
            case 2: // If it's number
                if ( i > 0) // After 'code' and later goes indent
                {
                    oss << " ";
                }
                oss << "0x" << std::hex << this->dmp[ i].num << std::dec; // HEX
                /* Do we need to put comma? */
                if ( ( i > 0) && ( i < 4) && ( this->dmp[ i + 1].type != 0))
                {
                    oss << ",";
                }
                break;
            default:
                assert( 0); // Error
        }
    }
    return oss.str();
}

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr)
{
    /** Reload operator only calls Dump method. */
    return out << instr.Dump( "");
}
