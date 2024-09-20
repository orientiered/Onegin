#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
//#define DEBUG_PRINTS
#include "error_debug.h"
#include "sorters.h"
#include "mystring.h"
#include "utils.h"
#include "onegin.h"

static void insertionSortBase(void *array, size_t elemSize, size_t alignment, size_t length, cmpFuncPtr_t cmp);
static ullPair_t quickSortPartition(void *array, size_t elemSize, size_t length, void* pivot, cmpFuncPtr_t cmp);
static ullPair_t quickSortPartitionFast(void *array, size_t elemSize, size_t length, void* pivot, cmpFuncPtr_t cmp);

void bubbleSort(void *array, size_t length, size_t elemSize, cmpFuncPtr_t cmp) {
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


void shellSort(void *array, size_t length, size_t elemSize, cmpFuncPtr_t cmp) {
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

void insertionSort(void *array, size_t length, size_t elemSize, cmpFuncPtr_t cmp) {
    insertionSortBase(array, elemSize, elemSize, length, cmp);
}

void insertionSortSwapless(void *array, size_t length, size_t elemSize, cmpFuncPtr_t cmp) {
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

#define GET_ELEM(array, i) ((char *) (array) + elemSize * (i))

void quickSort(void *array, size_t length, size_t elemSize, cmpFuncPtr_t cmp) {
    DBG_PRINTF("length = %ld\n", length);
    MY_ASSERT(array, return);
    if (length <= 15) {
        shellSort(array, length, elemSize, cmp);
        return;
    }
    // if (length <= 1) return;
    // if (length == 2) { // TODO: small array?
    //     if (cmp(array, (char*) array + elemSize) > 0)
    //         swap(array, (char*) array + elemSize, elemSize);
    //     return;
    // }

    char *pivot = GET_ELEM(array, length / 2);
    ullPair_t sep = quickSortPartition(array, length, elemSize, pivot, cmp);

// array        array[length-1]
//  |                |
//  x...x||||||x.....x
//       ^     ^
//  sep.first  sep.second
//  x - new boundaries of qsort
    quickSort(array, (size_t) sep.first, elemSize, cmp);
    quickSort(GET_ELEM(array, sep.second), (size_t)(length - sep.second), elemSize, cmp);
}

static ullPair_t quickSortPartition(void *array, size_t length, size_t elemSize, void* pivot, cmpFuncPtr_t cmp) {
    swap(array, pivot, elemSize); //moving pivot element to start of the array
    // <<<<<<=====......>>>>
    //       ^    ^    ^
    //     left head right
    // <=> - elements <=> pivot(left);
    // . - elements, that are not partitioned yet
    char    *left  = GET_ELEM(array, 0),
            *head  = GET_ELEM(array, 1),
            *right = GET_ELEM(array, length-1);
    int cmpResult = 0;
    while (head <= right) {
        cmpResult = cmp(left, head);
        if (cmpResult < 0) {
            swap(head, right, elemSize);
            right -= elemSize;
        } else if (cmpResult > 0) {
            swap(left, head, elemSize);
            left += elemSize;
            head += elemSize;
        } else {
            head += elemSize;
        }
    }
    MY_ASSERT(left <= GET_ELEM(array, length-1), abort());
    MY_ASSERT(head <= GET_ELEM(array, length), abort());
    ullPair_t separator = {(size_t)(left - (char*)array) / elemSize, (size_t)(head - (char*)array) / elemSize};
    return separator;
}

static ullPair_t quickSortPartitionFast(void *array, size_t length, size_t elemSize, void* pivot, cmpFuncPtr_t cmp) {
    char *pivotElem = (char *) calloc(1, elemSize);
    memcpy(pivotElem, pivot, elemSize);

    char *left  = GET_ELEM(array, 0),
         *right = GET_ELEM(array, length-1);
    while (1) {
        while (cmp(pivotElem, right) < 0)
            right -= elemSize;

        while (cmp(pivotElem, left) > 0)
            left += elemSize;

        if (left >= right) {
            free(pivotElem);
            MY_ASSERT(right <= GET_ELEM(array, length-1), abort());
            MY_ASSERT(right >= GET_ELEM(array, 0), abort());
            ullPair_t separator = { (size_t)(right - (char*)array) / elemSize,
                                    (size_t)(right - (char*)array) / elemSize + 1};
            return separator;
        }
        swap(left, right, elemSize);
        left += elemSize;
        right -= elemSize;
    }
    free(pivotElem);
    MY_ASSERT(0, fprintf(stderr, "Logic error in qsort partition\n"); abort(););
    ullPair_t separator = {0, 0};
    return separator;
}

#undef GET_ELEM

int ullCmp(const void* first, const void* second) {
    typedef const unsigned long long cull_t;
    return int (*(cull_t *) first - *(cull_t *) second);
}

int stringArrCmp(const void *firstStr, const void *secondStr) {
    MY_ASSERT(firstStr, return 0);
    MY_ASSERT(secondStr, return 0);
    return strcmp(((const string_t *)firstStr)->str, ((const string_t *)secondStr)->str);
}

int stringArrAlphaCmp(const void *firstStr, const void *secondStr) {
    MY_ASSERT(firstStr, return 0);
    MY_ASSERT(secondStr, return 0);
    return stralphacmp(((const string_t *)firstStr)->str, ((const string_t *)secondStr)->str);
}

int stringArrCmpBackward(const void *firstStr, const void *secondStr) {
    MY_ASSERT(firstStr, return 0);
    MY_ASSERT(secondStr, return 0);
    return strcmpBackward(((const string_t *)firstStr)->str, ((const string_t *)secondStr)->str);
}

int stringArrCmpBackwardFast(const void *firstStr, const void *secondStr) { //relies on text_t structure
    MY_ASSERT(firstStr, return 0);
    MY_ASSERT(secondStr, return 0);
    const string_t  firstStrT = *(const string_t *) firstStr,
                    secondStrT = *(const string_t *) secondStr;
    const char  *fStart = firstStrT.str,
                *sStart = secondStrT.str;
    const char  *fStr = firstStrT.str + ((firstStrT.size > 0) ? firstStrT.size - 1 : 0),
                *sStr = secondStrT.str + ((secondStrT.size > 0) ? secondStrT.size - 1 : 0);

    for (; (fStr > fStart) && (sStr > sStart) && (*fStr == *sStr); fStr--, sStr--)
        ;
    if ((*fStr == *sStr) && ((fStr == fStart) ^ (sStr == sStart)))
        return int(fStr - fStart) - int(sStr - sStart);
    return int(*fStr) - int(*sStr);
}

int stringArrAlphaCmpBackward(const void *firstStr, const void *secondStr) {
    MY_ASSERT(firstStr, return 0);
    MY_ASSERT(secondStr, return 0);
    return stralphacmpBackward(((const string_t *)firstStr)->str, ((const string_t *)secondStr)->str);
}

int stringArrAlphaCmpBackwardFast(const void *firstStr, const void *secondStr) {
    MY_ASSERT(firstStr, return 0);
    MY_ASSERT(secondStr, return 0);
    const string_t  firstStrT = *(const string_t *) firstStr,
                    secondStrT = *(const string_t *) secondStr;
    const char  *fStart = firstStrT.str,
                *sStart = secondStrT.str;
    const char  *fStr = firstStrT.str + ((firstStrT.size > 0) ? firstStrT.size - 1 : 0),
                *sStr = secondStrT.str + ((secondStrT.size > 0) ? secondStrT.size - 1 : 0);

    while (fStr > fStart && sStr > sStart) {
        fStr = findAlphabetCharBackward(fStr, fStart);
        sStr = findAlphabetCharBackward(sStr, sStart);
        if (tolower(*fStr) != tolower(*sStr))
            return tolower(*fStr) - tolower(*sStr);
        if (fStr > fStart) fStr--;
        if (sStr > sStart) sStr--;
    }
    fStr = findAlphabetCharBackward(fStr, fStart);
    sStr = findAlphabetCharBackward(sStr, sStart);
    if ((tolower(*fStr) == tolower(*sStr)) && ((fStr == fStart) ^ (sStr == sStart)))
        return int(fStr - fStart) - int(sStr - sStart);
    return tolower(*fStr) - tolower(*sStr);
}



sortFuncPtr_t chooseSortFunction(const char *sortName) {
    //choose sort function based on it's name
    typedef struct {
        sortFuncPtr_t sortFunc;
        const char *sortName;
    } sortFuncAndName_t;
    const sortFuncAndName_t sortFunctions[] = {
        {bubbleSort,    "bubble"},
        {insertionSort, "insertion"},
        {shellSort,     "shell"},
        {quickSort,     "qsort"},
        {qsort,         "cqsort"}
    };
    const unsigned len = ARRAY_SIZE(sortFunctions);

    const unsigned defaultSort = 3; //qsort

    if (!sortName) {
        printf("Using default sort function: %s\n", sortFunctions[defaultSort].sortName);
        return sortFunctions[defaultSort].sortFunc;
    }

    for (unsigned index = 0; index < len; index++)
        if (strcmp(sortFunctions[index].sortName, sortName) == 0)
            return sortFunctions[index].sortFunc;

    printf("Using default sort function: %s\n", sortFunctions[defaultSort].sortName);
    return sortFunctions[defaultSort].sortFunc;
}
