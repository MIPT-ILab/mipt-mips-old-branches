#include <cstdlib>
#include <string>
#include <vector>
#include <cassert>
#include <types.h>
#include <func_memory.h>
#include <sstream>

using namespace std;


#ifndef NDEBUG
	#define ASSERT(cond, message)			\
		if (!(cond))				\
		{					\
			cerr << message 		\
			    << endl;  			\
			exit(EXIT_FAILURE);		\
		}
#else
	# define ASSERT(cond, message)
#endif


class FuncInstr
{
	public:
		FuncInstr(uint32 bytes);
		~FuncInstr();
		std::string Dump(std::string indent = "") ;
		void initformat(uint32 bytes);
		void parser(uint32 bytes);
		string initreg(uint8 num);

	
	enum Format
	{
		FORMAT_R,
		FORMAT_I,
		FORMAT_J,
		UNIX
	} format;
	
	int spformat;


	union 
	{
		struct
		{
			unsigned C:16;
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
		} asJ;

		
		uint32 raw;
	} info;
};

uint32 get_instr (uint8 *pointer);
		
