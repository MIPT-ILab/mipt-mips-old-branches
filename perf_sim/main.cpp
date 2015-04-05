/**
 * main.cpp - Main file implementing scalar MIPS simulator
 *            with constant latency for instruction execution
 * @author Alexander Pronin <alexander.pronin.mipt@gmail.com>
 * Copyright 2015 MIPT-MIPS
 */

#include <perf_sim.h>
#include <func_instr.h>
#include <elf_parser.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>

using namespace std;

int main( int argc, char** argv) {
    bool silent = true;
    int file = 1;

    if ( argc < 3 || argc > 4)
    {
        cout << "WRONG NUMBER OF ARGUMENT" << endl;
        return -1;
    }
    else
    {
        if ( argc == 4) {
            if ( strcmp( argv[ 1], "-d")) {
                cout << "UNKNOWN KEY" << endl;
                return -2;
            }
            else {
                file = 2;
                silent = false;
            }
        }
        PerfMIPS* mips = new PerfMIPS;
        mips->run( argv[ file], atoi( argv[ file + 1]), silent);
    }
    return 0;
}
