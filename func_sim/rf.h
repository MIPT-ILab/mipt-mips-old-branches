/*
 * rf.h - mips register file
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015 MIPT-MIPS 
 */

#ifndef RF_H
#define RF_H

#include <func_instr.h>

class RF
{
        struct Reg {
            uint32 value;
            bool   is_valid;
            Reg() : value(0ull), is_valid(true) { }    
        } array[REG_NUM_MAX];
    public:
        uint32 read( RegNum num) const {
            assert( array[(size_t)num].is_valid);
            return array[(size_t)num].value;
        }
        bool check( RegNum num) const { return array[(size_t)num].is_valid; }
        void invalidate( RegNum num) { array[(size_t)num].is_valid = false; }
        void validate( RegNum num) { array[(size_t)num].is_valid = true; }
        void write ( RegNum num, uint32 val) {
             assert( array[(size_t)num].is_valid);
             array[(size_t)num].value = val;
             array[(size_t)num].is_valid = true;
        }
    
    //    uint32 array[REG_NUM_MAX];
    //public:
        inline bool read_src1( FuncInstr& instr) const
        {
            RegNum reg_num = instr.get_src1_num();
            if ( check( reg_num))
                instr.set_v_src1( read( reg_num));
            else return false;
            return true;
        }

        inline bool read_src2( FuncInstr& instr) const
        {
            RegNum reg_num = instr.get_src2_num();
            if ( check( reg_num))
                instr.set_v_src2( read( reg_num));
            else return false;
            return true;
        }

        inline void write_dst( const FuncInstr& instr)
        {
            RegNum reg_num = instr.get_dst_num();
            if ( REG_NUM_ZERO != reg_num)
            {
                write( reg_num, instr.get_v_dst());
                validate( reg_num);
            }
        }
        
        inline void reset( RegNum reg)
        {
            if ( check( reg))
                write( reg, 0);
        }
 
        RF()
        {
            for ( size_t i = 0; i < REG_NUM_MAX; ++i)
                reset((RegNum)i);
        }
};
          
#endif
 
