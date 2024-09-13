#include <stdlib.h>
#include <stdio.h>
#include "sorters.h"
#include "mystring.h"
#include "utils.h"

//#define DEBUG_PRINTS
#include "error_debug.h"

static void insertionSortBase(void *array, size_t elemSize, size_t alignment, size_t length, cmpFuncPtr_t cmp);
static llPair_t quickSortPartition(void *array, size_t elemSize, size_t length, void* pivot, cmpFuncPtr_t cmp);
static llPair_t quickSortPartitionFast(void *array, size_t elemSize, size_t length, void* pivot, cmpFuncPtr_t cmp);

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
            insertionSortBase((char*)array + elemSize * offset, elemSize,
                              elemSize*steps[stepIndex], (length-offset) / steps[stepIndex], cmp);
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

    char *pivot = (char*) array + elemSize * (length / 2);
    llPair_t sep = quickSortPartition(array, elemSize, length, pivot, cmp);

    quickSort(array, elemSize, sep.first+1, cmp);
    quickSort((char*) array + elemSize * sep.second, elemSize, length - sep.second, cmp);
}

static llPair_t quickSortPartition(void *array, size_t elemSize, size_t length, void* pivot, cmpFuncPtr_t cmp) {
    swap(array, pivot, elemSize); //moving random element to start of the array
    char *sepLeft = (char*) array, *sepRight = (char*) array + elemSize; //[left;right) - block with equal elements
    char *left = sepRight, *right = (char*) array + elemSize * (length - 1); //for all i >= right array[i] > sepElement
    int cmpResult = 0;
    while (left <= right) {
        cmpResult = cmp(sepLeft, left);
        if (cmpResult < 0) {
            swap(left, right, elemSize);
            right -= elemSize;
        } else if (cmpResult > 0) {
            swap(sepLeft, left, elemSize);
            sepLeft += elemSize;
            sepRight += elemSize;
            left += elemSize;
        } else {
            sepRight += elemSize;
            left += elemSize;
        }
    }
    llPair_t separator = {(size_t)(sepLeft - (char*)array) / elemSize, (size_t)(sepRight - (char*)array) / elemSize};
    return separator;
}

static llPair_t quickSortPartitionFast(void *array, size_t elemSize, size_t length, void* pivot, cmpFuncPtr_t cmp) {
    char **pivotElem = (char **) pivot;
    char *left = (char*)array, *right = (char*) array + elemSize * (length - 1);
    while (1) {
        while (cmp(pivotElem, right) < 0)
            right -= elemSize;

        while (cmp(pivotElem, left) > 0)
            left += elemSize;

        if (left >= right) {
            llPair_t separator = {(size_t)(right - (char*)array) / elemSize, (size_t)(right - (char*)array) / elemSize + 1};
            return separator;
        }
        swap(left, right, elemSize);
        left += elemSize;
        right -= elemSize;
    }
    llPair_t separator = {0, 0};
    return separator;
}

int ullCmp(const void* first, const void* second) {
    return *(unsigned long long*) first - *(unsigned long long*) second;
}

int strvoidcmp(const void *firstStr, const void *secondStr) {
    return stralphacmp((const char*) firstStr, (const char*) secondStr);
}

int stringArrCmp(const void *firstStr, const void *secondStr) {
    MY_ASSERT(firstStr, return 0);
    MY_ASSERT(secondStr, return 0);
    return strcmp(*(char * const *)firstStr, *(char * const *)secondStr);
}

int stringArrAlphaCmp(const void *firstStr, const void *secondStr) {
    MY_ASSERT(firstStr, return 0);
    MY_ASSERT(secondStr, return 0);
    return stralphacmp(*(char * const *)firstStr, *(char * const *)secondStr);
}

int stringArrCmpBackward(const void *firstStr, const void *secondStr) {
    MY_ASSERT(firstStr, return 0);
    MY_ASSERT(secondStr, return 0);
    return strcmpBackward(*(char * const *)firstStr, *(char * const *)secondStr);
}

int stringArrAlphaCmpBackward(const void *firstStr, const void *secondStr) {
    MY_ASSERT(firstStr, return 0);
    MY_ASSERT(secondStr, return 0);
    return stralphacmpBackward(*(char * const *)firstStr, *(char * const *)secondStr);
}
sortFuncPtr_t chooseSortFunction(const char *sortName) {
    //choose sort function based on it's name
    const sortFuncPtr_t sortFunctions[] = {
        bubbleSort, insertionSort, shellSort, quickSort};
    const char *sortNames[] = {
        "bubble",   "insertion",   "shell",   "qsort"};
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
