#include <iostream>
#include <string>
#include <cstdlib>
#include <cassert>
#include <cstring>

#include "../../common/types.h"


using namespace std;


// If field "shamt" don't need, shamt = NO_SHAMT
static const uint32 NO_SHAMT = 0xFFFFFFFF;
// If field "funct" don't need, funct = NO_FUNCT
static const uint8 NO_FUNCT = 0xFF;
// If field "immidiate" don't need, imm = NO_IMM
static const uint32 NO_IMM = 0xFFFFFFFF;
// If instruction don't defined, opcode = INSTR_NOT_DEF
static const uint8 INSTR_NOT_DEF = 0xFF;
// Size of ISAarray
static const uint8 SIZE_OF_ARRAY = 12;

 
// Main struct, which have all information about instruction
struct InstrData
{
    uint8 opcode;
    uint8 funct;

	string format;
	string type;
};


// This union needs for reading binary instruction in comfortable format
union bytes_t
{

// For reading in I-type
	struct
	{
		unsigned imm:16;
		unsigned t:5;
		unsigned s:5;
		unsigned opcode:6;
	} asI;

// For reading in R-type
	struct
	{
		unsigned funct:6;
		unsigned shamt:5;
		unsigned d:5;
		unsigned t:5;
		unsigned s:5;
		unsigned opcode:6;
	} asR;

// For reading im J-type
	struct
	{
		unsigned imm:26;
		unsigned opcode:6;
	} asJ;

// For reading instruction in binary format
	uint32 raw;
};


// Main class, which have info about instruction and some methods, which use this instruction
class FuncInstr
{
	private:
// Binary instruction	
		bytes_t bytes_;

// Info about instruction
		InstrData InstrInfo;

// Static array for store ISA information
        static const InstrData isaTable[ ]; 

// This array have all registers name, each register meet his number in registers file
        static const string registers[ 33];

// Operands of instruction
        string rs;
        string rt;
        string rd;

        uint32 shamt;
        uint32 imm;

	public:
// Constructor of this class. He is parse binary instruction and enter info about it in InstrInfo
		FuncInstr( uint32 bytes);

// Methods for parse
        void parse_R( );
        void parse_I( );
        void parse_J( );

// Method for find pseudo-instructions
        void pseudo( );

// Dump method. He is print instruction in assembler format
		string Dump( string indent = " ") const;

// This function just call Dump method with empty indent string
        friend ostream& operator<< ( ostream& out, const FuncInstr& instr)
        {
            out << instr.Dump( "");
            return out;
        };
};

