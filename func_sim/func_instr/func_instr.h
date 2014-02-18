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

// This array have all registers name, each register meet his number in registers file
static string registers[ 33] = 					
{								
	"$zero",						
	"$at",							
	"$v0", "$v1",						
	"$a0", "$a1", "$a2", "$a3",				
	"$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", 
	"$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", 
	"$t8", "$t9",						
	"$k0", "$k1",						
	"$gp",							
	"$sp", 							
	"$fp",							
	"$ra",							
	"NOTHING"						
};

// Main struct, which have all information about instruction
struct InstrData
{
    uint8 opcode;
    uint8 funct;
	uint32 shamt;

	string rs;
	string rt;
	string rd;
	
	uint32 imm;

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

	public:
// Constructor of this class. He is parse binary instruction and enter info about it in InstrInfo
		FuncInstr( uint32 bytes);
// Dump method. He is print instruction in assembler format
		string Dump( string indent = " ");
};
