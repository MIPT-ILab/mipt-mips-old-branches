/*
 * func_instr.h - instruction parser for mips
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2014 MIPT-MIPS
 */

#ifndef FUNC_INSTR_H
#define FUNC_INSTR_H

// Generic C++
#include <string>
#include <cassert>

// MIPT-MIPS modules
#include <types.h>
#include <elf_parser.h>

class FuncInstr
{
	private:
		enum Format
		{
			FORMAT_R,
			FORMAT_I,
			FORMAT_J,
			FORMAT_UNKNOWN
		} format;

		enum OperationType
		{
			OUT_R_ARITHM,
			OUT_R_SHAMT,
			OUT_R_JUMP,
			OUT_I_ARITHM,
			OUT_I_BRANCH,
			OUT_J_JUMP,
			OUT_I_LOAD,
			OUT_I_STORE,
			OUT_R_LOGIC,
			OUT_I_LOGIC,
			OUT_I_SHAMT
		} operation;

		union _instr
		{
			struct
			{
				unsigned funct  :6;
				unsigned shamt  :5;
				unsigned rd     :5;
				unsigned rt     :5;
				unsigned rs     :5;
				unsigned opcode :6;
			} asR;
			struct
			{
				unsigned imm    :16;
				unsigned rt     :5;
				unsigned rs     :5;
				unsigned opcode :6;
			} asI;
			struct
			{
				unsigned imm    :26;
				unsigned opcode :6;
			} asJ;
			uint32 raw;

			_instr(uint32 bytes) 
			{
				raw = bytes;
            }
        } instr;

		struct ISAEntry
		{
			std::string name;

			uint8 opcode;
			uint8 funct;

			Format format;
			OperationType operation;
			
			void ( FuncInstr::*toFunction)();    
		};
        
		uint32 isaNum;

		static const ISAEntry isaTable[];
		static const uint32 isaTableSize;
		static const char *regTable[];

		/* strings are necessary for dump method */ 
		
		std::string disasm;
		std::string disasmImm;
		std::string disasmShamt;
		std::string disasmrd;
		std::string disasmrs;
		std::string disasmrt;
		std::string dissrc1;
		std::string dissrc2;
		std::string disdst;
		std::string disasmName;
		
		void initFormat();
		void initR();
		void initI();
		void initJ();
		void initUnknown();
		
		int write_back;       // this variable controls wb method of class MIPS
 		
	public:
		FuncInstr( uint32 bytes, uint32 PC = 0);
		std::string Dump( std::string indent = " ") const;
		
		void parseInstr(); // this function needs to parse instruction for dump 
		
		/* functions for execution by CPU */
		
		void add(); 
		void sub(); 
		void subu();	
		void addu();
		void addiu();
		void sll();
		void srl();
		void beq();
		void bne();
		void addi();
		void j();
		void jr();
		void andOp();
		void orOp();
		void xorOp();
		void andi();
		void ori();
		void xori();
		void lui();
		void lb();
		void lw();
		void lh();
		void lbu();
		void lhu();
		void sb();
		void sw();
		void sh();
		
		void execute();           
		
		/* get index of sources and destination register */
		
		int get_src1_num_index() const;
		int get_src2_num_index() const;
		int get_dst_num_index()  const;
		
		/* get constant values from instruction */
		
		int getImm();
		int getShamt();
		int getJImm();
		
		/* this function returns value of write_back variable of FuncnIstr class*/ 
		
		int getWB() const;
		
		const uint32 PC;
		uint32 new_PC;
		uint32 v_src1;
		uint32 v_src2;
		uint32 v_dst;
		uint32 CJump;
		uint32 C;
		uint32 Sh;
		uint32 mem_addr;
		
		void (FuncInstr::*pointer)(); // pointer to function of execution
		
		
};

std::ostream& operator<<( std::ostream& out, FuncInstr& instr);   // overloading of "<<" operator 

#endif //FUNC_INSTR_H

