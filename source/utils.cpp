#include <stdlib.h>
#include "utils.h"
void swap(void* a, void* b, size_t len) {

    if ((((size_t) a) % 8) != (((size_t) b) % 8)) {
        swapByByte(a, b, len);
        return;
    }

    const unsigned blockSize = sizeof(long long);
    const unsigned startOffset = (blockSize - ((size_t) a % 8)) % 8;
    swapByByte(a, b, startOffset);

    size_t llSteps = (len-startOffset) / sizeof(long long);

    long long *lla = (long long*) ((size_t)a + startOffset), *llb = (long long*) ((size_t)b + startOffset);
    long long temp = 0;

    while (llSteps--) {
        temp = *lla;
        *lla++ = *llb;
        *llb++ = temp;
    }

    swapByByte(lla, llb, (len-startOffset) % 8);
}

void swapByByte(void* a, void* b, size_t len) {
    char *ac = (char*) a, *bc = (char*) b;
    char c = 0;
    while (len--) {
        c = *ac;
        *ac++ = *bc;
        *bc++ = c;
    }
}
