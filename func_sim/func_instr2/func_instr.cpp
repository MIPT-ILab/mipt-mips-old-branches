/**
 * implementation of class FuncInstr
 *
 *
 */

#include "func_instr.h"
#include <sstream>
#include "commands.cpp"

FuncInstr::FuncInstr( uint32 bytes)
{
    this->bytes.raw = bytes;
    parse_command(textAss, this->bytes);
}

std::string FuncInstr::Dump( std::string indent) const
{
    return indent+this->textAss;
}

std::ostream& operator << ( std::ostream& out, const FuncInstr& instr)
{
    return out << instr.Dump("");
}

