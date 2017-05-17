/*******************************************************

    Usage of a functional simulator

    @file: main.cpp
    @date: March 7, 2015
    @author: Kirill Korolev <kirill.korolef@gmail.com>
    @vertion: 1.0 (March 7, 2015) 

*******************************************************/

#include <func_sim.h>

int main(int argc, char **argv)
{
	if (argc != 3)
		printf("main: bad number of parameters");

	MIPS *mips = new MIPS;
	mips->run(argv[1], atoi(argv[2]));
	return 0;
}
