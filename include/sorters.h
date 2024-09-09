/// @file
/// @brief Sort functions and comparators

#ifndef SORTERS_H
#define SORTERS_H

typedef void(*sortFuncPtr_t)(void *array, size_t elemSize, size_t length, int (*cmp)(const void *first, const void *second));

/*
    @brief Simple bubble sort

    @param array Pointer to start of the array
    @param elemSize Size of element in array
    @param length Number of elements to sort
    @param cmp Pointer to comparator function

*/
void bubbleSort(void *array, size_t elemSize, size_t length, int (*cmp)(const void *first, const void *second));

/*
    @brief Simple insertion sort

    @param array Pointer to start of the array
    @param elemSize Size of element in array
    @param length Number of elements to sort
    @param cmp Pointer to comparator function

*/
void insertionSort(void *array, size_t elemSize, size_t length, int (*cmp)(const void *first, const void *second));

void insertionSortSwapless(void *array, size_t elemSize, size_t length, int (*cmp)(const void *first, const void *second));

void shellSort(void *array, size_t elemSize, size_t length, int (*cmp)(const void *first, const void *second));

void quickSort(void *array, size_t elemSize, size_t length, int (*cmp)(const void *first, const void *second));

/*
    @brief Comparator to sort array of strings (char**) using stralphacmp
*/
int stringArrayCmp(const void *firstStr, const void *secondStr);

/*
    @brief Just stralphacmp function
*/
int strvoidcmp(const void *firstStr, const void *secondStr);

#endif
