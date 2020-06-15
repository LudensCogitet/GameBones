#ifndef GB_UTILS_H_INCLUDED
#define GB_UTILS_H_INCLUDED

#include <stdint.h>

void binString(uint64_t value, char *buffer, unsigned int bufferSize);
void printBinString(uint64_t value, unsigned int bufferSize, const char *suffix);

#endif // GB_UTILS_H_INCLUDED
