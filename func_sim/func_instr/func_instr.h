#ifndef FUNC_INSTR_H_
#define FUNC_INSTR_H_

//uArch headers
#include <types.h>
#include <elf_parser.h>

//My headers


//C++ headers
#include <iostream>
#include <string>
#include <cassert>

//constants
const uint16 ISANum = 12;
const uint16 PINum = 3;
const uint8 REGNum = 32; 
const FAIL = 0;
const JUMP = 10;//number of jump comand in ISATable

//********************************************************************

class FuncInstr
{
private:
	enum Format
    	{
        	FORMAT_R,
        	FORMAT_I,
        	FORMAT_J
    	} format;
	
	enum Type_C
	{
		CAL;
		SHIFT;
		BRANCH_C;
		BRANCH_UC;
	} type_c;
	
	union
	{
        	struct
        	{
            		unsigned imm : 16;
              		unsigned t : 5;
                        unsigned s : 5;  
                        unsigned opcode : 6;          	
        	} asI;
        	struct
        	{       unsigned func : 6;
                        unsigned sh : 5;
                        unsigned d : 5;
                        unsigned t : 5;	
                        unsigned s : 5;
			unsigned opcode : 6;		
        	} asR;
        	struct
        	{
       			unsigned addr : 26;
                        unsigned opcode : 6;
        	} asJ;
        	uint32 raw;
    	} bytes;

	struct ISAEntry
    	{
        	const char* name;

        	uint8 opcode;
        	uint8 func;

        	FuncInstr::FormatType format;
        	FuncInstr::Type type;
    	};
	struct PIComand
	{
		const char* name;
	};
	struct REG
	{
		const char* name;
	};

    	static const ISAEntry isaTable[];
	static const PIComand PiTable[];
	static const REG REGFile[]; 
	std :: string DumpCom; 
public:
	FuncInstr ( uint32 bytes );
	void initFormat ( uint32 bytes );
        void init ();
        void parseR ( uint32 bytes );
        void parseI ( uint32 bytes );
        void parseJ ( uint32 bytes );
	std :: string Dump ( std :: string indent = " " ) const;
};

std :: ostream& operator << ( std :: ostream &out, const FunckInstr & instr );



#endif 
