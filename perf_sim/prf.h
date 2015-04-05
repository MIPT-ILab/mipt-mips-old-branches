/*
 * rf.h - mips register file
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015 MIPT-MIPS 
 */

#ifndef RF_H
#define RF_H

#include <func_instr.h>

const uint32 reg_max_num = 32;
class RF
{
        struct Reg {
            uint32 value;
            bool   is_valid;
            Reg() : value(0ull), is_valid(true) { /*cerr << "+";*/ }    
        }  array[reg_max_num];

    public:
        inline void read_src1( FuncInstr& instr) const
        {
           RegNum reg_num = instr.get_src1_num();
           uint32 val = read( reg_num);
           instr.set_v_src1( val);
        }

        inline void read_src2( FuncInstr& instr) const
        {
           RegNum reg_num = instr.get_src2_num();
           uint32 val = read( reg_num);
           instr.set_v_src2( val);
        }

        inline void write_dst( const FuncInstr& instr)
        {
            RegNum reg_num = instr.get_dst_num();
            if ( REG_NUM_ZERO != reg_num)
                write( reg_num, instr.get_v_dst());
        }

        inline void reset( RegNum reg)
        {
            write( reg, 0);
        }
        //-->
        uint32 read( const RegNum num) const {
               assert( array[(size_t) num].is_valid == true);
               return array[(size_t) num].value;
        }
        bool check( RegNum num) const { return array[(size_t) num].is_valid; }
        void invalidate( const RegNum num) { if  ( num )  array[(size_t) num].is_valid = false; }
        void write ( const RegNum num, uint32 val) {
            if  ( num) {
                assert( array[(size_t)num].is_valid == false);
                array[(size_t)num].value = val;
                array[(size_t)num].is_valid = true;
            }
        }
        //--<
        RF()
        {
            //cerr << "RF is construct\n";
        }
};
          
#endif
 

