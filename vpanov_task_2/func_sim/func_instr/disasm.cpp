#include <iostream>
using namespace std;
#include <func_instr.h>

int main() {
    FuncInstr a(0x20FFFFFF);
    cout << a.Dump();
    return 0;
}