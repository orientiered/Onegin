#include <stdlib.h>
#include "sorters.h"
#include "mystring.h"
#include "utils.h"

void bubbleSort(void *array, int elemSize, int length, int (*cmp)(const void *first, const void *second)) {
    for (int rightBoundary = length; rightBoundary > 0; rightBoundary--) {
        unsigned char swapFlag = 0; //if we didn't swap anything, array is sorted
        for (int i = 0; i < rightBoundary-1; i++) {
            if (cmp((char*)array + i*elemSize, (char*)array + (i+1)*elemSize) > 0) {
                swapFlag = 1;
                swap((char*)array + i*elemSize, (char*)array + (i+1)*elemSize, elemSize);
            }
        }
        if (!swapFlag) break;
    }
}


int strvoidcmp(const void *firstStr, const void *secondStr) {
    return stralphacmp((const char*) firstStr, (const char*) secondStr);
}


int stringArrayCmp(const void *firstStr, const void *secondStr) {
    return stralphacmp(*(const char **)firstStr, *(const char**)secondStr);
}
