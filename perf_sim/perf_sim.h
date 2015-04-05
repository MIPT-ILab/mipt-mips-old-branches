/*
 * perf_sim.h - mips scalar simulator
 * @author Dmitry Ermilov ermilov@phystech.edu
 * For MIPT-MIPS 2015
 */

#ifndef FUNC_SIM_H
#define FUNC_SIM_H


#include <func_instr.h>
#include <func_memory.h>
#include <prf.h>
#include <ports.h>

class PerfMIPS
{
        RF*  rf;
        uint32  PC;
        FuncMemory*  mem;
        bool  PC_is_valid;
        bool  is_silent;
        uint32 executed_instrs;

        uint32 fetch() const { return mem->read(PC); }
        void read_src(FuncInstr& instr) const {
            rf->read_src1(instr); 
            rf->read_src2(instr); 
        }

        void load(FuncInstr& instr) const {
            instr.set_v_dst(mem->read(instr.get_mem_addr(), instr.get_mem_size()));
        }

        void store(const FuncInstr& instr) {
            mem->write(instr.get_v_src2(), instr.get_mem_addr(), instr.get_mem_size());
        }

        void load_store(FuncInstr& instr) {
            if (instr.is_load())
                load(instr);
            else if (instr.is_store())
                store(instr);
        }

        void wb(const FuncInstr& instr) {
            rf->write_dst(instr);
        }

        bool src_check( FuncInstr obj) const;
        bool dst_check( FuncInstr obj) const;

        std::string fetch_dump( int cycle);
        std::string master_dump( std::string module, std::string disasm, int cycle );

        //Fetch module
        WritePort<uint32>*  wp_fetch_2_decode;
        WritePort<bool>*    wp_decode_2_fetch_stall;

        uint32  fetch_data;      
        void clock_fetch(int cycle);

        //Decode module
        ReadPort<uint32>*      rp_fetch_2_decode;
        WritePort<FuncInstr>*  wp_decode_2_execute;
        ReadPort<bool>*        rp_decode_2_fetch_stall;
        WritePort<bool>*       wp_execute_2_decode_stall;

        uint32     decode_data;
        FuncInstr  decode_obj;  
        bool       prev_dec;
        void clock_decode( int cycle);

        //Execute module
        ReadPort<FuncInstr>*   rp_decode_2_execute;
        WritePort<FuncInstr>*  wp_execute_2_memory;
        ReadPort<bool>*        rp_execute_2_decode_stall;
        WritePort<bool>*       wp_memory_2_execute_stall;

        FuncInstr  execute_obj;
        bool prev_exe;
        void clock_execute( int cycle);


        //Memory module
        ReadPort<FuncInstr>*   rp_execute_2_memory;
        WritePort<FuncInstr>*  wp_memory_2_writeback;
        ReadPort<bool>*        rp_memory_2_execute_stall;
        WritePort<bool>*       wp_writeback_2_memory_stall;
;
        FuncInstr  memory_obj;
        bool prev_mem;
        void clock_memory( int cycle);

        //WriteBack module
        ReadPort<FuncInstr>*  rp_memory_2_writeback;
        ReadPort<bool>*       rp_writeback_2_memory_stall;

        FuncInstr  writeback_obj;
        bool prev_wb;
        void clock_writeback( int cycle);

    public:
        PerfMIPS();
        ~PerfMIPS();

        void run(const std::string& tr, uint32 instrs_to_run, bool silent = true);
};
            
#endif
 