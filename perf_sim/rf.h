/*
 * rf.h - mips register file
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * modified by Alexander Pronin <alexander.pronin.mipt@gmail.com>
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
            Reg() : value( 0ull), is_valid( true) { }    
        } array[ REG_MAX_NUM];
    public:
        uint32 read( Reg_Num num) { return array[ ( size_t) num].value; }

        bool check( Reg_Num num) const { return array[ ( size_t) num].is_valid; }

        void invalidate( Reg_Num num) { array[ ( size_t) num].is_valid = false; }   // zero register checks are in perf_mips

        void write ( Reg_Num num, uint32 val) {                                     // zero register checks are in perf_mips
            assert( array[(size_t)num].is_valid == false);
            array[ ( size_t) num].value    = val;
            array[ ( size_t) num].is_valid = true;
        }

        void reset( RegNum reg) {
            array[ reg].is_valid = true;
            array[ reg].value = 0;
        }
 
        RF() {
            for ( size_t i = 0; i < REG_NUM_MAX; ++i)
                reset( ( RegNum) i);
        }
};
          
#endif
 
