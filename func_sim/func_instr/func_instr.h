#include <iostream>
#include <string>
#include <cstdlib>
#include <cassert>

#include "types.h"
//#include <elf_parser.h>

using namespace std;

static const uint32 NO_SHAMT = 0xFFFFFFFF;
static const uint8 NO_FUNCT = 0xFF;
static const uint32 NO_IMM = 0xFFFFFFFF;
static const uint8 INSTR_NOT_DEF = 0xFF;

static string registers[33] = 					
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

union bytes_t
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
		unsigned imm:26;
		unsigned opcode:6;
	} asJ;
	uint32 raw;
};

class FuncInstr
{
	private:	
		bytes_t bytes_;
		InstrData InstrInfo;

	public:
		FuncInstr(uint32 bytes);
		string Dump(string indent = " ");
};

//static InstrData InstrArray[10];
