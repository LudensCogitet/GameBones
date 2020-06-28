#include "./gbResManagement.h"

unsigned int gbNextEmpty(void **resArray, unsigned int c, unsigned int max) {
    unsigned int cStart = c;
    if (c == max) c = 0;
    while (resArray[c] != 0) {
        if (++c == cStart) break;
        if (c == max) c = 0;
    }

    return c;
}
