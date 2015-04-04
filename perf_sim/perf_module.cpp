#include <perf_module.h>

void clock_module(int cycle){
        bool is_stall;
        /* If the next module tells us to stall, we stops
           and send stall signals to previous module */
        rp_next_2_me_stall->read( &is_stall, cycle);
        if ( is_stall) {
             wp_me_2_previous_stall->write( true, cycle);
             return;
        }

        /* If nothing cames from previous stage
           execute, memory and writeback modules have to jump out here */
        if ( rp_previous_2_me->read( &module_data, cycle))
            return;

        /* But, decode stage doesn't jump out
           It takes non-updated bytes from module_data
           and re-decodes them */
        // rp_previous_2_me->read( &module_data, cycle)

        // Here we process data.

        if (...) {
             /* This branch is chosen if everything is OK and
                we may continue promotion to the next pipeline stages */
             wp_me_2_next->write( module_data, cycle);
        }
        else {
             // Otherwise, nothing is done and we have to stall pipeline
             wp_me_2_previous_stall->write( true, cycle);
        }
}
