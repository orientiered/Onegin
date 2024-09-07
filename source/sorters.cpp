#include <stdlib.h>
#include "sorters.h"
#include "mystring.h"
#include "utils.h"

void bubbleSort(void *array, size_t elemSize, size_t length, int (*cmp)(const void *first, const void *second)) {
    for (size_t rightBoundary = length; rightBoundary > 0; rightBoundary--) {
        unsigned char swapFlag = 0; //if we didn't swap anything, array is sorted
        for (size_t i = 0; i < rightBoundary-1; i++) {
            if (cmp((char*)array + i*elemSize, (char*)array + (i+1)*elemSize) > 0) {
                swapFlag = 1;
                swap((char*)array + i*elemSize, (char*)array + (i+1)*elemSize, elemSize);
            }
        }
        if (!swapFlag) break;
    }
}

void insertionSort(void *array, size_t elemSize, size_t length, int (*cmp)(const void *first, const void *second)) {
    for (size_t index = 1; index < length; index++) {
        size_t pos = index;
        while (pos > 0 && cmp((char*)array + elemSize*pos, (char*)array + elemSize*(pos-1)) > 0) {
            swap((char*)array + elemSize*pos, (char*)array + elemSize*(pos-1), elemSize);
            pos--;
        }
    }
}

int strvoidcmp(const void *firstStr, const void *secondStr) {
    return stralphacmp((const char*) firstStr, (const char*) secondStr);
}


int stringArrayCmp(const void *firstStr, const void *secondStr) {
    return stralphacmp(*(const char **)firstStr, *(const char**)secondStr);
}
