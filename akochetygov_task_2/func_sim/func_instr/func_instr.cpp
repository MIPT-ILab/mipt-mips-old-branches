/**
 * func_instr.cpp - Implementation of FuncInstr class
 *@author Andrey Kochetygov <andrey.kochetygov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

//Generic
#include <assert.h>

//Generic C++
#include <iostream>
#include <string>
#include <sstream>

//uArchSim modules
#include <func_instr.h>
#include <types.h>

FuncInstr::command FuncInstr::allcommands[]=
{
    { "add" , add1 , RT , 0 , 20 , 3 },
    { "addu" , addu1 , RT , 0 , 21 , 3 },
    { "sub" , sub1 , RT , 0 , 22 , 3 },
    { "subu" , subu1 , RT , 0 , 23 , 3 },
    { "addi" , addi1 , IT , 8 , 0 , 2 },
    { "addiu" , addiu1 , IT , 9 , 0 , 2 },
    { "sll" , sll1 , RT , 0 , 0 , 2 },
    { "srl" , srl1 , RT , 0 , 2 , 2 },
    { "beq" , beq1 , IT , 4 , 0 , 1 },
    { "bne" , bne1 , IT , 5 , 0 , 1 },
    { "j" , j1 , JT , 2 , 0 , 0  },
    { "jr" , jr1 , RT , 0, 8 , 1 }
};

const uint32 number_of_commands = sizeof( FuncInstr::allcommands) / sizeof( FuncInstr::allcommands[0]);
const char* FuncInstr::allregisters[]={"$zero" , "$at" , "$vo" , "$v1" ,"$a0" ,
                    "$a1" , "$a2" , "$a3" , "$t0" , "$t1" , "$t2" , "$t3" , "$t4" ,
                     "$t5" , "$t6" , "$t7" ,"$s0" , "$s1" , "$s2" , "$s3" , "$s4" ,
                     "$s5" , "$s6" , "$s7" , "$t8" , "$t9" , "$k0" , "$k1"
                    };

void FuncInstr::RTdivider()
{
    funct = curr->RST.funct;
    shamt = curr->RST.shamt;
    dst = ( register_name)curr->RST.dst;
    trg = ( register_name)curr->RST.trg;
    src =( register_name) urr->RST.src;
    return;
}

void FuncInstr::ITdivider()
{
    constant = curr->IST.constant;
    trg = ( register_name)curr->IST.trg;
    src = (register_name)curr->IST.src;
    return;
}

void FuncInstr::JTdivider()
{
    jmp = curr->JST.jmp;
    return;
}

void FuncInstr::ispseudo()
{
  if(ecurrname == addiu1 && dst == 0)
  {
      pseudo = 1;
      return;
  }

  if(( ecurrname == addu1) && (dst == 0) && (src ==0) )
  {
      pseudo = 2;
      return;
  }
  if( ecurrname == sll1 && (dst==0) && (src == 0 ) && ( trg == 0) )
  {
      pseudo = 3;
      return;
  }
  pseudo = 0;
  return;
}

FuncInstr::FuncInstr( uint32 bytes)
{
    int i,j;
    code = bytes;
    curr = ( divide*) ( &bytes);
    funct = curr->RST.funct;
    opcode = curr->RST.opcode;
    isfound = 0;
    for( i = 0; i < number_of_commands; i++)
    {
        if (((allcommands[i].opcodecomm == opcode) && (opcode !=0 )) || ((opcode == 0)&&(funct == allcommands[i].funct)))
        {
            ift = allcommands[i].type;
            ecurrname = allcommands[i].instr_name;
            currname = allcommands[i].name;
            printform = allcommands[i].printform;
            i = number_of_commands;
            if(ift == RT)
                RTdivider();
            if(ift == IT)
                ITdivider();
            if(ift == JT)
                JTdivider();
            isfound = 1;
        }

    }
ispseudo();
}

std::string FuncInstr::Dump( std::string indent ) const
{
    std::ostringstream oss;
    if( isfound)

    {
        if ( pseudo == 0)
        {
            oss << std::hex<<currname<<" ";
            if ( printform == 1 && ift == IT)
            {
                oss<<allregisters[src]<<" "<<allregisters[trg];
                return oss.str();
            }
            if( printform == 1 && ift == RT )
            {
                oss<<allregisters[src];
                return oss.str();
            }
            if ( printform == 2 && ift == RT)
            {
                oss<<allregisters[dst]<<" "<<allregisters[trg]<<" "<<shamt;
                return oss.str();
            }
            if ( printform == 2 && ift == IT)
            {
                oss<<allregisters[trg]<<" "<<allregisters[src]<<" "<<constant;
                return oss.str();
            }
            if( printform == 3)
            {
                oss<<allregisters[dst]<<" "<<allregisters[src]<<" "<<allregisters[trg];
                return oss.str();
            }
            if ( printform == 0)
            {
                oss<<jmp;
                return oss.str();
            }
        }

        if ( pseudo == 1)
        {
            oss<<"move"<<" "<<allregisters[trg]<<" "<<allregisters[src];
            return oss.str();
        }
        if( pseudo == 2)
        {
            oss<<"clear"<<" "<<allregisters[trg];
            return oss.str();

        }
        if ( pseudo == 3)
        {
            oss<<"nop";
            return oss.str();
        }

    }   else
    {
      oss<<"instruction not found";
      return oss.str();
    }

}

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr)
{
    out<<instr.Dump("");
    return out;
}


