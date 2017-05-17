#include <func_sim.h>

int main(int argc, char** argv) {
    MIPS* mips = new MIPS;
    mips->run("/mnt/DiskD/mipt-task3/vpanov_task_3/tests/samples/static_arrays.out", 3);
    std::cout << mips->dump();
    delete mips;
    return 0;
}