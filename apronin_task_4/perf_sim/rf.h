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
        } array[ REG_NUM_MAX];
    public:

        uint32 read( RegNum num) { return array[ ( size_t) num].value; }                         // get data from register

        bool check( RegNum num) const { return array[ ( size_t) num].is_valid; }                 // check register validity

        void invalidate( RegNum num) { if ( num) array[ ( size_t) num].is_valid = false; }       // set invalid state to register

        void write ( RegNum num, uint32 val) {                                                   // write value to register
            if ( num) {
                assert( array[ ( size_t) num].is_valid == false);
                array[ ( size_t) num].value    = val;
                array[ ( size_t) num].is_valid = true;
            }
        }

        void reset( RegNum reg) {                                                                // reset register
            array[ reg].is_valid = true;
            array[ reg].value = 0;
        }
 
        RF() {                                                                                   // prepare register file
            for ( size_t i = 0; i < REG_NUM_MAX; ++i)
                reset( ( RegNum) i);
        }
};
          
#endif
 
