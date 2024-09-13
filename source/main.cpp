#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
//#define DEBUG_PRINTS
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

    checkNULLStrings(onegin); //searches for NULL strings in text
    FILE *outFile = stdout;
    if (flags[OUTPUT].set)
        outFile = fopen(flags[OUTPUT].val._string, "wb");
    if (!outFile) {
        fprintf(stderr, "Can't open output file\n");
        return 0;
    }

    sortFuncPtr_t sortFunc = quickSort;
    if (flags[SORT_ALG].set) {
        sortFunc = chooseSortFunction(flags[SORT_ALG].val._string);
    }

    pthread_t plotThread = 0;
    if (flags[SORT_TIME].set) {
        doublePair_t averageTime = sortTimeTest(50, onegin, sortFunc, stringArrayCmp, &plotThread);
        if (averageTime.first > 40000) //>40ms
            printf("Average sorting time is %.1f+-%.1f ms\n", averageTime.first / 1000, averageTime.second / 1000);
        else
            printf("Average sorting time is %.3f+-%.3f ms\n", averageTime.first / 1000, averageTime.second / 1000);
    }

    sortFunc(onegin.text, sizeof(char*), onegin.textLen, stringArrayCmp);

    if (checkIsSorted(onegin, stringArrayCmp))
        printf("Sort doesn't work\n");

    writeTextToFile(onegin, outFile); //forward sorting
    DBG_PRINTF("First write\n");

    sortFunc(onegin.text, sizeof(char*), onegin.textLen, stringArrayCmpBackward);
    writeTextToFile(onegin, outFile); //sorting from end of strings
    DBG_PRINTF("Second write\n");

    sortFunc(onegin.text, sizeof(char*), onegin.textLen, ullCmp);
    writeTextToFile(onegin, outFile); //original Text
    DBG_PRINTF("Third write\n");

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
