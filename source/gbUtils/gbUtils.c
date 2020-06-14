#include "./gbUtils.h"

void binString(uint64_t value, char *buffer, unsigned int size) {
    buffer[size - 1] = '\0';

    for (unsigned int i = size - 2; i < size - 1; i--) {
        buffer[(size - 2) - i] = '0' + ((value >> i) & 0x01);
    }
}
