#include "func_instr.h"

int main()
{
	FuncInstr instr1(0x016A4821);
	instr1.Dump();
	FuncInstr instr2(0x00108880);
	instr2.Dump();
	FuncInstr instr3(0x116C0064);
	instr3.Dump();
	return 0;
}
