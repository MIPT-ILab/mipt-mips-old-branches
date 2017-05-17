#include <iostream>

#include <perf_sim.h>

MIPS::MIPS():
instr_num(0),  
read_instr(0), 
executed_instr(0), 
cycle(0),
fetch_raw(0),
decode_raw(0),
fetch_empty(true),
decode_empty(true),
exec_empty(true)
{
    	rf = new RF();
	
	wp_fetch_2_decode =     new WritePort<uint32>   ("f_2_d", PORT_BW, PORT_FANOUT);
	wp_decode_2_execute =   new WritePort<FuncInstr>("d_2_e", PORT_BW, PORT_FANOUT);
	wp_execute_2_memory =   new WritePort<FuncInstr>("e_2_m",PORT_BW, PORT_FANOUT);
	wp_memory_2_writeback = new WritePort<FuncInstr>("m_2_w",PORT_BW, PORT_FANOUT);

	rp_fetch_2_decode =     new ReadPort<uint32>   ("f_2_d",PORT_LATENCY);
        rp_decode_2_execute =   new ReadPort<FuncInstr>("d_2_e",PORT_LATENCY);
	rp_execute_2_memory =   new ReadPort<FuncInstr>("e_2_m", PORT_LATENCY);
	rp_memory_2_writeback = new ReadPort<FuncInstr>("m_2_w", PORT_LATENCY);

	wp_decode_2_fetch_stall =     new WritePort<bool>("d_2_f_stall", PORT_BW, PORT_FANOUT);
	wp_execute_2_decode_stall =   new WritePort<bool>("e_2_d_stall",PORT_BW, PORT_FANOUT);
	wp_memory_2_execute_stall =   new WritePort<bool>("m_2_e_stall", PORT_BW, PORT_FANOUT);
	wp_writeback_2_memory_stall = new WritePort<bool>("w_2_m_stall", PORT_BW, PORT_FANOUT);

	rp_decode_2_fetch_stall =     new ReadPort<bool>("d_2_f_stall", PORT_LATENCY);
	rp_execute_2_decode_stall =   new ReadPort<bool>("e_2_d_stall", PORT_LATENCY);
	rp_memory_2_execute_stall =   new ReadPort<bool>("m_2_e_stall",PORT_LATENCY);
	rp_writeback_2_memory_stall = new ReadPort<bool>("w_2_m_stall", PORT_LATENCY);

	Port<FuncInstr>::init();
	Port<uint32>::init();
	Port<bool>::init();	
}

void MIPS::Fetch_Process(int cycle){
        bool is_stall = false;
        rp_decode_2_fetch_stall->read( &is_stall, cycle);
        if (is_stall || jump) {
		std::cout << "FETCH\t\tcycle " << cycle << ":\tstall" << endl;
		return;
	}
	
	FuncInstr instr;
	if (read_instr < instr_num) {
		fetch_raw = fetch();
		fetch_empty = false;
		read_instr++;
		instr = FuncInstr(fetch_raw, PC);
		if (!instr.is_jump()) PC += 4;
	}

	if (!fetch_empty) wp_fetch_2_decode->write(fetch_raw, cycle);

	std::cout  << "FETCH\t\tcycle " << cycle << ":\t0x" << hex << fetch_raw << endl;
}

void MIPS::Decode_Process(int cycle){
        bool is_stall = false;
	rp_execute_2_decode_stall->read( &is_stall, cycle);
	wp_decode_2_fetch_stall->write(is_stall, cycle);
        if (is_stall) {
		std::cout << "DECODE\t\tcycle " << cycle << ":\tstall" << endl;
		return;
	}

	FuncInstr instr;
	if (rp_fetch_2_decode->read( &decode_raw, cycle)) decode_empty = false;
	if (!decode_empty) instr = FuncInstr(decode_raw, PC);
	else return;
        wp_decode_2_execute->write(instr, cycle);

	std::cout << "DECODE\t\tcycle " << cycle << ":\t" << instr << endl;
}

void MIPS::Execute_Process(int cycle){
	bool is_stall = false;
	rp_memory_2_execute_stall->read(&is_stall, cycle);
	if (is_stall) {
		wp_execute_2_decode_stall->write(true, cycle);
		std::cout << "EXECUTE\t\tcycle " << cycle << ":\tstall" << endl;
		return;
	}

	if (rp_decode_2_execute->read(&exec_instr, cycle)) exec_empty = false;
	if (exec_empty) return;	

	if (!rf->check(exec_instr.get_src1_num()) ||
	    !rf->check(exec_instr.get_src2_num()) ) {
		wp_execute_2_decode_stall->write(true, cycle);
		std::cout << "EXECUTE\t\tcycle " << cycle << ":\tstall" << endl;
		return;
	}
	read_src(exec_instr);
	rf->invalidate(exec_instr.get_dst_num());
	exec_instr.execute();
	wp_execute_2_memory->write(exec_instr, cycle);

	std::cout << "EXECUTE\t\tcycle " << cycle << ":\t" << exec_instr << endl;
}

void MIPS::Memory_Process(int cycle){
	bool is_stall = false;
	
	rp_writeback_2_memory_stall->read(&is_stall, cycle);
	wp_memory_2_execute_stall->write(is_stall, cycle);
	if (is_stall) {
		std::cout << "MEMORY\t\tcycle " << cycle << ":\tstall" << endl;
		return;
	}

	FuncInstr instr;
	if (!rp_execute_2_memory->read(&instr, cycle)) return;
	load_store(instr);
	wp_memory_2_writeback->write(instr, cycle);

	std::cout << "MEMORY\t\tcycle " << cycle << ":\t" << instr << endl;
}

void MIPS::Writeback_Process(int cycle){
	wp_writeback_2_memory_stall->write(false, cycle);

	FuncInstr instr;
	if (!rp_memory_2_writeback->read( &instr, cycle)) return;
	wb(instr);
	if (instr.is_jump()) PC = instr.get_new_PC();
	executed_instr++;
	std::cout << "WRITEBACK\tcycle " << cycle << ":\t" << instr << std::endl;
}

void MIPS::run(const std::string& tr, uint32 instrs_to_run)
{
	instr_num = instrs_to_run;
    mem = new FuncMemory(tr.c_str());
    PC = mem->startPC();
    
	while (executed_instr < instrs_to_run) {
		Fetch_Process(cycle);
		Decode_Process(cycle);
		Execute_Process(cycle);
		Memory_Process(cycle);
		Writeback_Process(cycle);
		++cycle;
    }
    delete mem;
}

MIPS::~MIPS() {
	delete rf;
	delete wp_fetch_2_decode;
	delete rp_decode_2_fetch_stall;	
        delete wp_decode_2_execute;
	delete rp_fetch_2_decode;
        delete rp_execute_2_decode_stall;
	delete wp_decode_2_fetch_stall;
        delete wp_execute_2_memory;
        delete rp_decode_2_execute;
        delete rp_memory_2_execute_stall;
       	delete wp_execute_2_decode_stall;
        delete wp_memory_2_writeback;
	delete rp_execute_2_memory;
       	delete rp_writeback_2_memory_stall;
	delete wp_memory_2_execute_stall;
        delete rp_memory_2_writeback;
        delete wp_writeback_2_memory_stall;
}
