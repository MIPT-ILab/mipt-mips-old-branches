/**
 * extra_functions.h - Header of module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Semyon Abramov <semyon.abramov.mipt@gmail.com>
 */
#ifndef EXTRA_FUNCTIONS
#define EXTRA_FUNCTIONS


vector<int> toBin( uint64 val);
vector<int> toBin( uint8 val);
uint64 fromBin( vector<int> data);
void myWrite( uint8* content,
              uint64 value,
              uint64 from,
              uint64 count);

#endif // #ifndef EXTRA_FUNCTIONS
