#include "./gbUtils.h"

void binString(uint64_t value, char *buffer, unsigned int bufferSize) {
    buffer[bufferSize - 1] = '\0';

    for (unsigned int i = bufferSize - 2; i < bufferSize - 1; i--) {
        buffer[(bufferSize - 2) - i] = '0' + ((value >> i) & 0x01);
    }
}

void printBinString(uint64_t value, unsigned int bitLength, const char *suffix) {
    char buffer[bitLength + 1];
    binString(value, &buffer, bitLength + 1);
    printf("%s%s", buffer, suffix);
}
