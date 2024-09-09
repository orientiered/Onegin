#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "error_debug.h"
#include "mystring.h"
#include "utils.h"
#include "argvProcessor.h"
#include "sorters.h"
#include "oneginIO.h"


int check(char **array, size_t length, int (*cmp)(const void *a, const void *b)) {
    printf("Checking array\n");
    for (size_t i = 0; i < length-1; i++)
        if (cmp(array + i, array + i + 1) > 0) return 1;

    return 0;
}

int main(int argc, char *argv[]) {
    argVal_t flags[argsSize] = {};
    initFlags(flags);
    processArgs(flags, argc, argv);

    if (flags[HELP].set) {
        printHelpMessage();
        return 0;
    }

    const char *fileName = "Romeo and Juliet.txt";
    if (flags[INPUT].set)
        fileName = flags[INPUT].val._string;

    char **textStrings = NULL;
    size_t stringsCnt = 0;
    if (readFromFile(fileName, &textStrings, &stringsCnt) != GOOD_EXIT) {
        fprintf(stderr, "Can't read from file\n");
        return 0;
    }
    char *fullText = textStrings[0]; //this pointer must be freed at the end

    sortFuncPtr_t sortFunc = shellSort;
    if (flags[SORT_ALG].set) {
        if (strcmp("bubble", flags[SORT_ALG].val._string) == 0)
            sortFunc = bubbleSort;
        else
        if (strcmp("insertion", flags[SORT_ALG].val._string) == 0)
            sortFunc = insertionSort;
        else
        if (strcmp("shell", flags[SORT_ALG].val._string) == 0)
            sortFunc = shellSort;
        else
        if (strcmp("qsort", flags[SORT_ALG].val._string) == 0)
            sortFunc = quickSort;
        else {
            printf("Using default sort: insertionSort\n");
        }
    }
    clock_t startTime = clock();
    sortFunc(textStrings, sizeof(char*), stringsCnt, stringArrayCmp);
    clock_t endTime = clock();
    if (check(textStrings, stringsCnt, stringArrayCmp))
        printf("Sort doesn't work\n");

    if (flags[SORT_TIME].set)
        printf("Sorting took %ld ms\n", (endTime-startTime)*1000/CLOCKS_PER_SEC);
    FILE *outFile = stdout;
    if (flags[OUTPUT].set)
        outFile = fopen(flags[OUTPUT].val._string, "wb");
    if (!outFile) {
        fprintf(stderr, "Can't open output file\n");
        return 0;
    }
    writeStringsToFile(textStrings, stringsCnt, outFile);

    free(fullText);
    free(textStrings);
    fclose(outFile);
    return 0;
}
