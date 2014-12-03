#include <iostream>
using namespace std;
#include <func_instr.h>

int main() {
    FuncInstr a(0x23bd0008);
    cout << a.Dump();
    return 0;
}