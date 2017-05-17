#include "func_instr.h"

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>


const FuncInstr::ISAEntry FuncInstr::isaTable[] = {
  { "add", 0x0, 0x20, FuncInstr::FORMAT_R, FuncInstr::ADD}, 
  { "addu", 0x0, 0x21, FuncInstr::FORMAT_R, FuncInstr::ADDU}, 
  { "sub", 0x0, 0x22, FuncInstr::FORMAT_R, FuncInstr::SUB}, 
  { "subu", 0x0, 0x23, FuncInstr::FORMAT_R, FuncInstr::SUBU}, 
  { "addi", 0x8, 0x0, FuncInstr::FORMAT_I, FuncInstr::ADDI}, 
  { "addiu", 0x9, 0x0, FuncInstr::FORMAT_I, FuncInstr::ADDIU}, 
  { "sll", 0x0, 0x0, FuncInstr::FORMAT_R, FuncInstr::SLL}, 
  { "srl", 0x0, 0x2, FuncInstr::FORMAT_R, FuncInstr::SRL}, 
  { "beq", 0x4, 0x0, FuncInstr::FORMAT_I, FuncInstr::BEQ}, 
  { "bne", 0x5, 0x0, FuncInstr::FORMAT_I, FuncInstr::BNE}, 
  { "j", 0x2, 0x0, FuncInstr::FORMAT_J, FuncInstr::J}, 
  { "jr", 0x0, 0x8, FuncInstr::FORMAT_R, FuncInstr::JR}
};

const FuncInstr::Register FuncInstr::registersTable[] = {
  { 0, "$zero"}, 
  { 1, "$at"}, 
  { 8, "$t0"}, 
  { 9, "$t1"},
  { 10, "$t2"},
  { 11, "$t3"},
  { 12, "$t4"},
  { 13, "$t5"},
  { 14, "$t6"},
  { 15, "$t7"}, 
  { 24, "$t8"}, 
  { 25, "$t9"}
};


uint32 FuncInstr::getBits( uint32 bytes, uint8 lesser, uint8 larger) const {
  return ( bytes & ( ( ( uint32) 1 << larger) - ( uint32) 1)) >> lesser;
}

void FuncInstr::initFormat( uint32 bytes) {
  this->bytes.raw = bytes;
  
  uint8 opcode = this->getBits( bytes >> 26, 0, 6);
  
  this->format = this->getCommandFormatType( opcode);
}

FuncInstr::FuncInstr( uint32 bytes) {
  this->initFormat( bytes);
  switch (this->format) {
    case FORMAT_R:
      this->parseR( bytes);
      break;
    case FORMAT_I:
      this->parseI( bytes);
      break;
    case FORMAT_J:
      this->parseJ( bytes);
      break;
    default:
      std::cerr << "ERROR.";
      exit(EXIT_FAILURE);
  }
}

void FuncInstr::parseR( uint32 bytes) {
  this->type = this->getCommandType( this->bytes.asR.opcode, this->bytes.asR.func);
  this->SVal = this->bytes.asR.shamt;
  this->dReg = this->bytes.asR.d;
  this->tReg = this->bytes.asR.t;
  this->sReg = this->bytes.asR.s;
}

void FuncInstr::parseI( uint32 bytes) {
  this->type = this->getCommandType( this->bytes.asI.opcode, 0x0);
  this->CVal = this->bytes.asI.imm;
  this->tReg = this->bytes.asI.t;
  this->sReg = this->bytes.asI.s;
}

void FuncInstr::parseJ( uint32 bytes) {
  this->type = this->getCommandType( this->bytes.asJ.opcode, 0x0);
  this->CVal = this->bytes.asJ.addr;
}

std::string FuncInstr::getRegisterName( uint8 number) const {
  uint8 array_size = sizeof(this->registersTable) / sizeof(FuncInstr::Register);
  
  
  for ( uint8 index = 0; index < array_size; ++index) {
    if ( this->registersTable[ index].number == number) {
      return std::string( this->registersTable[ index].name);
    }
  }
  
  std::cerr << "ERROR.";
  exit(EXIT_FAILURE);
}

std::string FuncInstr::getCommandName( FuncInstr::Type type) const {
  uint32 array_size = sizeof(this->isaTable) / sizeof(FuncInstr::ISAEntry);
  
  
  for ( uint32 index = 0; index < array_size; ++index) {
    if ( this->isaTable[ index].type == type) {
      return std::string( this->isaTable[ index].name);
    }
  }
  
  std::cerr << "ERROR.";
  exit(EXIT_FAILURE);
}

FuncInstr::FormatType FuncInstr::getCommandFormatType( uint8 opcode) const {
  uint32 array_size = sizeof(this->isaTable) / sizeof(FuncInstr::ISAEntry);
  
  
  for ( uint32 index = 0; index < array_size; ++index) {
    if ( this->isaTable[ index].opcode == opcode) {
      return this->isaTable[ index].format;
    }
  }
  
  std::cerr << "ERROR.";
  exit(EXIT_FAILURE);
}

FuncInstr::Type FuncInstr::getCommandType( uint8 opcode, uint8 func) const {
  uint32 array_size = sizeof(this->isaTable) / sizeof(FuncInstr::ISAEntry);
  
  
  for ( uint32 index = 0; index < array_size; ++index) {
    if ( this->isaTable[ index].opcode == opcode && 
         this->isaTable[ index].func == func) {
      return this->isaTable[ index].type;
    }
  }
  
  std::cerr << "ERROR.";
  exit(EXIT_FAILURE);
}

std::string FuncInstr::Dump( std::string indent) const {
  std::ostringstream result;
  
  
  result << indent << this->getCommandName( this->type) << " ";
  
  switch (this->format) {
    case FuncInstr::FORMAT_R:
      switch (this->type) {
        case FuncInstr::ADD:
        case FuncInstr::ADDU:
        case FuncInstr::SUB:
        case FuncInstr::SUBU:
          result << this->getRegisterName(this->dReg) << ", " << 
                    this->getRegisterName(this->sReg) << ", " << 
                    this->getRegisterName(this->tReg);
          break;
        case FuncInstr::SLL:
        case FuncInstr::SRL:
          result << this->getRegisterName(this->dReg) << ", " << 
                    this->getRegisterName(this->tReg) << ", " << 
                     "0x" << std::hex << this->CVal;
          break;
        case FuncInstr::JR:
          result << this->getRegisterName(this->sReg);
          break;
      }
      break;
    case FuncInstr::FORMAT_I:
      switch (this->type) {
        case FuncInstr::ADDI:
        case FuncInstr::ADDIU:
          result << this->getRegisterName(this->tReg) << ", " << 
                    this->getRegisterName(this->sReg) << ", " << 
                    "0x" << std::hex << this->CVal;
          break;
        case FuncInstr::BEQ:
        case FuncInstr::BNE:
          result << this->getRegisterName(this->sReg) << ", " << 
                    this->getRegisterName(this->tReg) << ", " << 
                    "0x" << std::hex << this->CVal;
          break;
      }
      break;
    case FuncInstr::FORMAT_J:
      result << "0x" << std::hex << this->CVal;
      break;
  }
  
  return result.str();
}

std::ostream &operator << ( std::ostream &out, const FuncInstr &instr) {
  out << instr.Dump( "");
  return out;
}