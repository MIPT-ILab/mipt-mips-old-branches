/*
 * rf.h - mips register file
 * @author Semyon Abramov <semyon.abramov.mipt@gmail.com>
 * 2015 iLab MIPT-MIPS project 
 */

#ifndef RF_H
#define RF_H

#include <func_instr.h>

class RF
{
        struct Reg 
		{
            uint32 value;
            bool is_valid;
			bool is_same;
            Reg() : value( 0ull), is_valid( true),is_same( false) { }    
		    inline void reset( )
            {
                this->value = 0;
                this->is_valid = true;
                this->is_same = false;
            }
        } array[REG_NUM_MAX];
	
    public:
        
        bool checkValid( RegNum num) const { return array[(size_t)num].is_valid; }
        bool checkSame( RegNum num) const { return array[(size_t)num].is_same; }
        void invalidate( RegNum num) 
        { 
            if ( num != REG_NUM_ZERO)
            {
			    array[ (size_t)num].is_valid = false; 
            }
        }
        
        void makeSame( RegNum num) { array[ (size_t)num].is_same = true; }
        void makeDifferent( RegNum num) { array[ (size_t)num].is_same = false;}
		
        inline void read_src1( FuncInstr& instr) const
        {
           size_t reg_num = instr.get_src1_num();
           instr.set_v_src1( array[reg_num].value);
        }

        inline void read_src2( FuncInstr& instr) const
        {
           size_t reg_num = instr.get_src2_num();
           instr.set_v_src2( array[reg_num].value);
        }

        inline void write_dst( const FuncInstr& instr)
        {
            
            size_t reg_num = instr.get_dst_num();
            
            if ( REG_NUM_ZERO != reg_num)
            {    
                if ( !instr.isJump())
                {	
				    array[reg_num].value = instr.get_v_dst();
                }
            }
            array[(size_t)reg_num].is_valid = true;
        }
		
        RF()
        {
            for ( size_t i = 0; i < REG_NUM_MAX; ++i)
            {
                array[i].reset( );
            }
        }
};

 
          
#endif
 
