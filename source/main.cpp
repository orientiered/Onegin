#include <stdio.h>
#include <stdlib.h>
#include "error.h"
#include "mystring.h"
#include "utils.h"
#include "argvProcessor.h"
#include "sorters.h"
#include "oneginIO.h"


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

    sortFuncPtr_t sortFunc = insertionSortSwapless;
    if (flags[SORT_ALG].set) {
        if (strcmp("bubble", flags[SORT_ALG].val._string) == 0)
            sortFunc = bubbleSort;
        else {
            printf("Using default sort: insertionSort\n");
        }
    }
    sortFunc(textStrings, sizeof(char*), stringsCnt, stringArrayCmp);

    FILE *outFile = stdout;
    if (flags[OUTPUT].set)
        outFile = fopen(flags[OUTPUT].val._string, "w");
    if (!outFile) {
        fprintf(stderr, "Can't open output file\n");
        return 0;
    }
    printText(textStrings, stringsCnt, outFile);

    free(fullText);
    free(textStrings);
    fclose(outFile);
    return 0;
}
