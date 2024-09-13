/// @file
/// @brief Sort functions and comparators

#ifndef SORTERS_H
#define SORTERS_H

typedef int (*cmpFuncPtr_t)(const void *first, const void *second);
typedef void(*sortFuncPtr_t)(void *array, size_t elemSize, size_t length, cmpFuncPtr_t cmp);

/*
    @brief Simple bubble sort

    @param array Pointer to start of the array
    @param elemSize Size of element in array
    @param length Number of elements to sort
    @param cmp Pointer to comparator function

*/
void bubbleSort(void *array, size_t elemSize, size_t length, cmpFuncPtr_t cmp);

/*
    @brief Simple insertion sort

    @param array Pointer to start of the array
    @param elemSize Size of element in array
    @param length Number of elements to sort
    @param cmp Pointer to comparator function

*/
void insertionSort(void *array, size_t elemSize, size_t length, cmpFuncPtr_t cmp);

void insertionSortSwapless(void *array, size_t elemSize, size_t length, cmpFuncPtr_t cmp);

void shellSort(void *array, size_t elemSize, size_t length, cmpFuncPtr_t cmp);

void quickSort(void *array, size_t elemSize, size_t length, cmpFuncPtr_t cmp);

/*
    @brief Comparator to sort array of strings (char**) using stralphacmp
*/
int stringArrayCmp(const void *firstStr, const void *secondStr);
int stringArrayCmpBackward(const void *firstStr, const void *secondStr);

/*
    @brief Just stralphacmp function
*/
int strvoidcmp(const void *firstStr, const void *secondStr);

int ullCmp(const void* first, const void* second);

sortFuncPtr_t chooseSortFunction(const char *sortName);

#endif
