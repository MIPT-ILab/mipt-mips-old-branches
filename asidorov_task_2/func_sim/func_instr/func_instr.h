// protection from multi-include
#ifndef FUNC_INSTR__FUNC_INSTR_H
#define FUNC_INSTR__FUNC_INSTR_H


#include "../../common/types.h"
#include <iostream>
#include <string>


class FuncInstr {
public:
  enum FormatType {
    FORMAT_R, 
    FORMAT_I, 
    FORMAT_J
  } format;
  
  enum Type {
    ADD, 
    ADDU, 
    SUB, 
    SUBU, 
    ADDI, 
    ADDIU, 
    SLL, 
    SRL, 
    BEQ, 
    BNE, 
    J, 
    JR
  } type;
  
  uint8 dReg;
  uint8 sReg;
  uint8 tReg;
  uint8 SVal;
  uint32 CVal;
  
  
  struct ISAEntry {
    const char *name;
    
    uint8 opcode;
    uint8 func;
    
    FuncInstr::FormatType format;
    FuncInstr::Type type;
  };
  
  struct Register {
    uint8 number;
    const char *name;
  };  
  
  static const ISAEntry isaTable[];
  static const Register registersTable[];
  
  
  FuncInstr( uint32 bytes);
  std::string Dump( std::string indent = " ") const;

private:
  union {
    struct {
      unsigned imm:16;
      unsigned t:5;
      unsigned s:5;
      unsigned opcode:6;
    } asI;
    
    struct {
      unsigned func:6;
      unsigned shamt:5;
      unsigned d:5;
      unsigned t:5;
      unsigned s:5;
      unsigned opcode:6;
    } asR;
    
    struct {
      unsigned addr:26;
      unsigned opcode:6;
    } asJ;
    
    uint32 raw;
  } bytes;
  
  
  // [lesser; larger)
  uint32 getBits( uint32 bytes, uint8 lesser, uint8 larger) const;
  std::string getRegisterName( uint8 number) const;
  std::string getCommandName( FuncInstr::Type type) const;
  FuncInstr::FormatType getCommandFormatType( uint8 opcode) const;
  FuncInstr::Type getCommandType( uint8 opcode, uint8 func) const;
  
  void initFormat( uint32 bytes);
  void parseR( uint32 bytes);
  void parseI( uint32 bytes);
  void parseJ( uint32 bytes);
};

std::ostream &operator << ( std::ostream &out, const FuncInstr &instr);


#endif // #ifndef FUNC_INSTR__FUNC_INSTR_H