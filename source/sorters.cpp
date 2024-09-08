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
        char* pos = (char*)array + elemSize*index;
        while (pos > array && cmp(pos, pos - elemSize) < 0) {
            swap(pos, pos-elemSize, elemSize);
            pos -= elemSize;
        }
    }
}

void insertionSortSwapless(void *array, size_t elemSize, size_t length, int (*cmp)(const void *first, const void *second)) {
    void *tempElem = calloc(1, elemSize);
    for (size_t index = 1; index < length; index++) {
        char *pos = (char*) array + index*elemSize;

        if (cmp(pos, pos - elemSize) < 0) {
            memcpy(tempElem, pos, elemSize);
            do {
                memcpy(pos, pos - elemSize, elemSize);
                pos -= elemSize;
            } while (pos > array && cmp(tempElem, pos-elemSize) < 0);
            memcpy(pos, tempElem, elemSize);
        }
    }
    free(tempElem);
}

int strvoidcmp(const void *firstStr, const void *secondStr) {
    return stralphacmp((const char*) firstStr, (const char*) secondStr);
}


int stringArrayCmp(const void *firstStr, const void *secondStr) {
    return stralphacmp(*(const char **)firstStr, *(const char**)secondStr);
}
