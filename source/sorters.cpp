#include <stdlib.h>
#include <stdio.h>
#include "sorters.h"
#include "mystring.h"
#include "utils.h"

//#define DEBUG_PRINTS
#include "error_debug.h"

static void insertionSortBase(void *array, size_t elemSize, size_t alignment, size_t length, cmpFuncPtr_t cmp);
static llPair_t quickSortPartition(void *array, size_t elemSize, size_t length, void* pivot, cmpFuncPtr_t cmp);

void bubbleSort(void *array, size_t elemSize, size_t length, cmpFuncPtr_t cmp) {
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


void shellSort(void *array, size_t elemSize, size_t length, cmpFuncPtr_t cmp) {
    // https://oeis.org/A036562
    const size_t steps[] = {1, 8, 23, 77, 281, 1073, 4193, 16577, 65921, 262913, 1050113, 4197377, 16783361, 67121153, 268460033, 1073790977};
    const short stepsSize = (short)sizeof(steps)/sizeof(size_t);

    for (short stepIndex = stepsSize-1; stepIndex >= 0; stepIndex--) {
        if (steps[stepIndex] > length) continue;
        for (size_t offset = 0; offset < steps[stepIndex]; offset++)
            insertionSortBase((char*)array + elemSize * offset, elemSize, elemSize*steps[stepIndex], (length-offset) / steps[stepIndex], cmp);
    }
}

void insertionSort(void *array, size_t elemSize, size_t length, cmpFuncPtr_t cmp) {
    insertionSortBase(array, elemSize, elemSize, length, cmp);
}

void insertionSortSwapless(void *array, size_t elemSize, size_t length, cmpFuncPtr_t cmp) {
    //version without swap() function. Moving elements instead
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

static void insertionSortBase(void *array, size_t elemSize, size_t alignment, size_t length, cmpFuncPtr_t cmp) {
    //alignment can be used to sort every n'th element
    for (size_t index = 1; index < length; index++) {
        char* pos = (char*)array + alignment*index;
        while (pos > array && cmp(pos, pos - alignment) < 0) {
            swap(pos, pos-alignment, elemSize);
            pos -= alignment;
        }
    }
}

void quickSort(void *array, size_t elemSize, size_t length, cmpFuncPtr_t cmp) {
    DBG_PRINTF("length = %ld\n", length);
    MY_ASSERT(array, return);
    if (length <= 1) return;
    if (length == 2) {
        if (cmp(array, (char*) array + elemSize) > 0)
            swap(array, (char*) array + elemSize, elemSize);
        return;
    }
    //TODO: sepElem = mean(start, mean, end)
    char *pivot = (char*) array + elemSize*(abs(rand()) % length);
    llPair_t sep = quickSortPartition(array, elemSize, length, pivot, cmp);

    quickSort(array, elemSize, sep.first+1, cmp);
    quickSort((char*) array + elemSize * sep.second, elemSize, length - sep.second, cmp);
}

static llPair_t quickSortPartition(void *array, size_t elemSize, size_t length, void* pivot, cmpFuncPtr_t cmp) {
    swap(array, pivot, elemSize); //moving random element to start of the array
    long long sepLeft = 0, sepRight = 1; //[left;right) - block with equal elements
    long long left = sepRight, right = length - 1; //for all i >= right array[i] > sepElement
    while (left <= right) {
        int cmpResult = cmp((char*)array + elemSize * sepLeft, (char*) array + elemSize * left);
        if (cmpResult < 0) {
            swap((char*) array + elemSize * left, (char*) array + elemSize * right, elemSize);
            right--;
        } else if (cmpResult > 0) {
            swap((char*) array + elemSize * sepLeft, (char*) array + elemSize * left, elemSize);
            sepLeft++;
            sepRight++;
            left++;
        } else {
            sepRight++;
            left++;
        }
    }
    llPair_t separator = {sepLeft, sepRight};
    return separator;
}

int ullCmp(const void* first, const void* second) {
    unsigned long long  stFirst  = *(unsigned long long*) first,
                        stSecond = *(unsigned long long*) second;

    return stFirst - stSecond;
}

int strvoidcmp(const void *firstStr, const void *secondStr) {
    return stralphacmp((const char*) firstStr, (const char*) secondStr);
}
int stringArrayCmp(const void *firstStr, const void *secondStr) {
    MY_ASSERT(firstStr, return 0);
    MY_ASSERT(secondStr, return 0);
    return stralphacmp(*(const char **)firstStr, *(const char**)secondStr);
}

int stringArrayCmpBackward(const void *firstStr, const void *secondStr) {
    MY_ASSERT(firstStr, return 0);
    MY_ASSERT(secondStr, return 0);
    return stralphacmpBackward(*(const char **)firstStr, *(const char**)secondStr);
}

sortFuncPtr_t chooseSortFunction(const char *sortName) {
    //choose sort function based on it's name
    const sortFuncPtr_t sortFunctions[] = {
        bubbleSort, insertionSort, shellSort, quickSort
    };
    const char *sortNames[] = {
        "bubble",   "insertion",   "shell",   "qsort"
    };
    const unsigned sortFunctionsLen = sizeof(sortFunctions)/sizeof(sortFuncPtr_t);
    const unsigned defaultSortIndex = 3; //qsort

    if (!sortName) {
        printf("Using default sort function: %s\n", sortNames[defaultSortIndex]);
        return sortFunctions[defaultSortIndex];
    }

    for (unsigned index = 0; index < sortFunctionsLen; index++)
        if (strcmp(sortNames[index], sortName) == 0)
            return sortFunctions[index];

    printf("Using default sort function: %s\n", sortNames[defaultSortIndex]);
    return sortFunctions[defaultSortIndex];
}
