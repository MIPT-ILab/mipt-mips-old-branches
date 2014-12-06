#include <string>
#include <vector>
#include <cassert>
#include <types.h>
#include <func_memory.h>


#ifndef NDEBUG
	#define ASSERT(cond, messege)			\
		if (!(cond))				\
		{					\
			printf ("Fatal error: %s, "	\
				"file: %s, line:%d \n", \
			#messege, __FILE__, __LINE__);  \
			abort();			\
		}
#else
	# define ASSERT(cond, messege)
#endif


class FuncInstr
{
	public:
		FuncInstr(uint32 bytes);
		std::string Dump(std::string indent = "") const;
		void FuncInstr::initFormat(uint32 bytes);
		void FuncInstr::parser(uint32 bytes);
		char FuncInstr::initreg(uint32 num);

	
	enum Format
	{
		FORMAT_R,
		FORMAT_I,
		FORMAT_J
	} format;
	
	int spformat;


	union 
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
			unsigned f:6;
			unsigned S:5;
			unsigned d:5;
			unsigned t:5;
			unsigned s:5;
			unsigned opcode:6;
	
		} asR;
		
		struct
		{
			unsigned A:26; 
			unsigned opcode:6;
		} asJ

		
		uint 32 raw;
	} info;
};

		
