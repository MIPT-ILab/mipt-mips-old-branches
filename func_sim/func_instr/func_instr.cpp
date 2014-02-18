//My headers
#include <func_instr.h>

//uArch Headers
#include <types.h>

//C++ headers
#include <cassert>
#include <iostream>
#include <string>

//********************************************************************

static const FuncInstr :: ISAEntry FuncInstr :: isaTable[ISANum] =
{
    	// name      opcode     func          format              type
	{ "add ",	0x0,	0x20, FuncInstr :: FORMAT_R, FuncInstr :: CAL },
	{ "addu ",	0x0,	0x21, FuncInstr :: FORMAT_R, FuncInstr :: CAL },
	{ "sub ",	0x0,	0x22, FuncInstr :: FORMAT_R, FuncInstr :: CAL },
	{ "subu ",	0x0,	0x23, FuncInstr :: FORMAT_R, FuncInstr :: CAL },
	{ "addi ",	0x8,	0x00, FuncInstr :: FORMAT_I, FuncInstr :: CAL },
	{ "addiu ",	0x9,	0x00, FuncInsrt :: FORMAT_I, FuncInstr :: CAL },
	{ "sll ",	0x0,	0x00, FuncInstr :: FORMAT_R, FuncInstr :: SHIFT },
	{ "srl ",	0x0,	0x02, FuncInstr :: FORMAT_R, FuncInstr :: SHIFT },
	{ "beq ",	0x4,	0x00, FuncInstr :: FORMAT_I, FuncInstr :: BRANCH_C },
	{ "bne ",	0x5,	0x00, FuncInstr :: FORMAT_I, FuncInstr :: BRANCH_C },
	{ "j ",		0x2,	0x00, FuncInstr :: FORMAT_J, FuncInstr :: BRANCH_UC },
	{ "jr ",		0x0,	0x08, FuncInstr :: FORMAT_R, FuncInstr :: BRANCH_UC }
};

//********************************************************************

static const FuncInstr :: PIComand FuncInstr :: PITable[PICNum] =
{
	"move ",
	"clear ",
	"nop"
};

//********************************************************************

static const FuncInstr :: REG FuncInstr :: REGFile [REGNum] =
{
	"$zero",						// const = 0
	"$at",							// assembler temporary
	"$v0", "$v1",						//
        "$a0", "$a2", "$a3",					//
        "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",	//temporaries
        "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",	//
        "$t8", "$t9",						//temporaries
        "$k0", "$k1",						//
        "$gp",							//
        "$sp",							//
        "$fp",							//
        "$ra" 							//
}

//********************************************************************

FuncInstr :: FuncInstr ( uint32 bytes )
{
    	this -> initFormat ( bytes );
    	switch ( this -> format )
    	{
        case FORMAT_R:
        	this -> parseR ( bytes );
            	break;
        case FORMAT_I:
            	this -> parseI ( bytes );
            	break;
        case FORMAT_J:
            	this -> parseJ ( bytes );
            	break;
        default:
            	assert ( "FuncInstr ( uint32 )" == FAIL );
    	}	
}

//********************************************************************

void FuncInstr :: initFormat ( uint32 bytes )
{
	this -> bytes.raw = bytes;
	uint8 opcode = this -> bytes.opcode;

	switch ( opcode )
	{
	case 0x0:
		this -> format = FORMAT_R;
		break;
	case 0x2:
		this -> format = FORMAT_J;
		break;
	case 0x4: case 0x5: case 0x8: case 0x9:
		this -> format = FORMAT_I;
		break;
	default:
		std :: cerr << "ERROR!: I don't know this opcode\n";
		assert ( "initFormat ( uint32 )" == FAIL );
	}	
} 

//********************************************************************

void FuncInstr :: parseR ( uint32 bytes )
{
	std :: ostringstream oss;

	for ( int i = 0; i < ISANum; ++i)
	{
		if ( this -> bytes.asR.func == this -> isaTable[i].func )
		{
			if ( this -> bytes.raw == 0)
				oss << PITable[2];
			else
			if ( ( this -> bytes.asR.func == 0x21 ) && 
			     ( this -> bytes.asR.t == 0 ) && 
			     ( this -> bytes.asR.s == 0 ) )
			{
				oss << PITable[1]  
				<< REGFile[this -> bytes.asR.d];
			}
			else
			{
				oss << this -> ISATable[i].name << " ";
				
				switch ( this -> ISATable[i].type )
				{
				case CAL:
					oss << REGFile[this -> bytes.asR.d] 
					    << ", " 
					    << REGFile[this -> bytes.asR.s] 
					    << ", "
					    << REGFile[this -> bytes.asR.t];
					break;	
				case SHIFT:
					oss << REGFile[this -> bytes.asR.d]
					    << ", "
					    << REGFile[this -> bytes.asR.t]
					    << ", "
					    << this -> bytes.asR.sh;
					break;
				case BRANCH_UC:
					oss << REGFile[this -> bytes.asR.s];
					break;
				default:
					std :: cerr << "ERROR!: Who knows?\n" 
						    << "How i got here?\n";
					assert ( "ERROR type FORMAT_R" == FAIL );
					break;
				}
			}
			
			oss << "\n";
			this -> DumpCom = oss.str();
			return;	
		}
	}
	
	std :: cerr << "ERROR!: invalid comand\n";
	assert ( "parseR ( uint32 )" == FAIL );	
}

//********************************************************************

void FuncInstr :: parseI ( uint32 bytes )
{
	std :: ostringstream oss;

	for ( int i = 0; i < ISANum; ++i )
	{
		if ( this -> bytes.asI.opcode == this -> isaTable[i].opcode )
		{
 			if ( !this -> bytes.asI.imm )
			{
				oss << this -> PITable[0]
				    << this -> bytes.asI.t
				    << ", "
				    << this -> bytes.asI.s;
			}
			else
			{
				oss << this -> isaTable[i].name 
                		    << this -> REGFile[this -> bytes.asI.s]
				    << ", "
                		    << this -> REGFile[this -> bytes.asI.t]
				    << ", "
                		    << this -> bytes.asI.imm;
			}
			oss << "\n";
			this DumpCom = oss.str();
			return;		
		}
	}	
	std :: cerr << "ERROR!: invalid comand\n"
	assert ( "parseI ( uint32 )" == FAIL );	
}

//********************************************************************

void FuncInstr :: parseJ ( uint32 bytes )
{
	std :: ostringstream oss;
	oss << this -> isaTable[JUMP].name
	    << this -> bytes.asJ.addr << "\n";	
}

//********************************************************************
