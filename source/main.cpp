#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define DEBUG_PRINTS
#include "error_debug.h"
#include "mystring.h"
#include "utils.h"
#include "argvProcessor.h"
#include "onegin.h"
#include "sorters.h"
#include "metrics.h"
#include "oneginIO.h"


void checkNULLStrings(text_t text);
int checkIsSorted(text_t textInfo, cmpFuncPtr_t cmp);

int main(int argc, char *argv[]) { //TODO: const char *argv[]
    argVal_t flags[argsSize] = {};
    initFlags(flags);
    processArgs(flags, argc, argv);

    if (flags[HELP].set) {
        printHelpMessage();
        return 0;
    }

    text_t onegin = {};

    const char *fileName = "OneginText.txt";
    if (flags[INPUT].set)
        fileName = flags[INPUT].val._string;

    if (readTextFromFile(fileName, &onegin) != GOOD_EXIT) {
        fprintf(stderr, "Can't read from file\n");
        return 0;
    }

    // assert(checkNULLStrings(onegin), true); //searches for NULL strings in text
    FILE *outFile = stdout;
    if (flags[OUTPUT].set)
        outFile = fopen(flags[OUTPUT].val._string, "wb");
    if (!outFile) {
        fprintf(stderr, "Can't open output file\n");
        return 0;
    }

    sortFuncPtr_t sortFunc = quickSort;
    cmpFuncPtr_t cmpFunc[] = {stringArrAlphaCmp, stringArrCmpBackward, ullCmp};

    if (flags[SORT_ALG].set) {
        sortFunc = chooseSortFunction(flags[SORT_ALG].val._string);
    }

    pthread_t plotThread = 0;
    if (flags[SORT_TIME].set) {
        doublePair_t averageTime = sortTimeTest(50, onegin, sortFunc, cmpFunc[0], &plotThread);
        if (averageTime.first > 40000) //>40ms
            printf("Average sorting time is %.1f+-%.1f ms\n", averageTime.first / 1000, averageTime.second / 1000);
        else
            printf("Average sorting time is %.3f+-%.3f ms\n", averageTime.first / 1000, averageTime.second / 1000);
    }

    for (size_t cmpIndex = 0; cmpIndex < sizeof(cmpFunc)/sizeof(cmpFuncPtr_t); cmpIndex++) {
        sortFunc(onegin.text, sizeof(char*), onegin.textLen, cmpFunc[cmpIndex]);
        #ifndef NDEBUG
        if (checkIsSorted(onegin, cmpFunc[cmpIndex]))
            printf("Sort doesn't work\n");
        #endif
        writeTextToFile(onegin, outFile); //forward sorting
        DBG_PRINTF("%u write\n", cmpIndex + 1);

    }

    deleteText(&onegin);

    if (outFile && outFile != stdout)
        fclose(outFile);

    pthread_join(plotThread, NULL);
    return 0;
}


int checkIsSorted(text_t textInfo, cmpFuncPtr_t cmp) {
    //printf("Checking array\n");
    for (size_t i = 0; i < textInfo.textLen-1; i++)
        if (cmp(textInfo.text + i, textInfo.text + i + 1) > 0) {
            DBG_PRINTF("%s\n%s\n", *(textInfo.text + i), *(textInfo.text + i + 1));
            return 1;
        }

    return 0;
}

void checkNULLStrings(text_t text) {
    //debug function to detect broken text
    for (size_t i = 0; i < text.textLen; i++) {
        if (!text.text[i]) {
            fprintf(stderr, "NULL STRING: %lu index, len %lu\n", i, text.textLen);
        }
    }
}
