/* disasm.cpp - module implementing 
 * reading and decoding every word from 
 * desired section
 */

 // Generic C++
 #include <string>
 #include <iostream>
 #include <iomanip>

 // Generic C
 #include <assert.h>
 #include <stdlib.h>
 #include <string.h>

 // uArchSim modules
 #include <elf_parser.h>
 #include <func_instr.h>

 using namespace std;

 int main( int argc, char* argv[])
 {
 	const int num_of_args = 3;
 	const string stand_indent = "    ";

 	if ( argc < num_of_args) 
 		{
 			cerr << "Error: Incorrect input" << endl; 
 			exit(1);
 		};
 	ElfSection section ( argv[1], argv[2]);	
 	cout << "Disassembly of the section " << argv[2] << ":" << endl << stand_indent;
 	
 	uint64 instr_addr;
 	
 	for ( instr_addr = section.startAddr(); section.isInside ( instr_addr, 4); instr_addr += 4)
 	{
 		uint32 instr_bytes = section.read ( instr_addr);
 		FuncInstr instr( instr_bytes);
 		cout << hex << instr_addr << ":" << stand_indent 
 		<< hex << instr_bytes << stand_indent << instr.Dump( stand_indent) << endl;
 	};

 	return 0;
 }
