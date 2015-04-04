#ifndef PERF_MODULE_H
#define PERF_MODULE_H

/*
void fetch_clock( int cycle);
void decode_clock( int cycle);
void execute_clock( int cycle);
void memory_clock( int cycle);
void writeback_clock( int cycle);
*/

template <typename rp_p2m_type, wp_m2n_type>
class PerfMIPS_module {
    ReadPort<rp_p2m_type>* rp_previous_2_me;
    ReadPort<bool>* rp_next_2_me_stall;

    WritePort<wp_m2n_type>* wp_me_2_next;
    WritePort<bool>* wp_me_2_previous_stall;

    //void *(*clock_module)(int *);
    void clock_module();


    PerfMIPS_module(ReadPort<rp_p2m_type>* rp_p2m,
                    WritePort<wp_m2n_type>* wp_m2n,
                    ReadPort<bool>* rp_n2m_s,
                    WritePort<bool>* wp_m2p_s/*,
                    void *(*clock_module_init)(int *)*/){
        rp_previous_2_me = rp_p2m;
        wp_me_2_next = wp_m2n;

        rp_next_2_me_stall = rp_n2m_s;
        wp_me_2_previous_stall = wp_m2p_s;
    }

};

#endif PERF_MODULE_H
