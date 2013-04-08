#include <iostream>
#include <bitset>

int main()
{
    const int size = 10;
    std::bitset< size> mybits;
    mybits.reset();
    mybits.set( 1);
    mybits.set( 2);
    std::cout << mybits.to_ulong() << std::endl;
    
}
