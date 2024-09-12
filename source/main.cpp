#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#define DEBUG_PRINTS
#include "error_debug.h"
#include "mystring.h"
#include "utils.h"
#include "argvProcessor.h"
#include "onegin.h"
#include "sorters.h"
#include "oneginIO.h"


void checkNULLStrings(text_t text);
int checkIsSorted(text_t textInfo, cmpFuncPtr_t cmp);
doublePair_t sortTimeTest(unsigned testNumber, text_t onegin, sortFuncPtr_t sortFunc, cmpFuncPtr_t cmp);
void percentageBar(unsigned value, unsigned maxValue, unsigned points, long long timePassed);

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

    if (flags[SORT_TIME].set) {
        doublePair_t averageTime = sortTimeTest(15, onegin, sortFunc, stringArrayCmp);
        if (averageTime.first > 40000) //>40ms
            printf("Average sorting time is %g+-%g ms\n", averageTime.first / 1000, averageTime.second / 1000);
        else
            printf("Average sorting time is %g+-%g mcs\n", averageTime.first, averageTime.second);
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

doublePair_t sortTimeTest(unsigned testNumber, text_t onegin, sortFuncPtr_t sortFunc, cmpFuncPtr_t cmp) {
    clock_t startTime = 0, endTime = 0;
    clock_t totalTime = 0;
    runningSTD(0, 1); //reseting runningSTD
    printf("Testing:\n");
    for (unsigned test = 0; test < testNumber; test++) {
        percentageBar(test, testNumber, 12, totalTime);
        startTime = clock();
        sortFunc(onegin.text, sizeof(char*), onegin.textLen, cmp);
        endTime = clock();
        runningSTD(endTime - startTime, 0);
        quickSort(onegin.text, sizeof(char*), onegin.textLen, ullCmp);
        percentageBar(test+1, testNumber, 12, totalTime);
        totalTime += clock() - startTime;
    }
    printf("\n");
    //returning average time in microseconds
    doublePair_t result = runningSTD(0, 1);
    result.first  *= 1000.0*1000/CLOCKS_PER_SEC;
    result.second *= 1000.0*1000/CLOCKS_PER_SEC;
    return result;
}

void percentageBar(unsigned value, unsigned maxValue, unsigned points, long long timePassed) {
    printf("\r[");
    for (unsigned i = 0; i < points; i++) {
        if (double(value) / maxValue > double(i) / points)
            printf("#");
        else
            printf("-");
    }
    printf("] %5.1f%%", double(value)/maxValue * 100);
    if (value > 0 && timePassed > 0) {
        printf(" Remaining time: %4.1f s", double(timePassed) / value * (maxValue - value) / CLOCKS_PER_SEC);
    }
    fflush(stdout);
}

void checkNULLStrings(text_t text) {
    for (size_t i = 0; i < text.textLen; i++) {
        if (!text.text[i]) {
            fprintf(stderr, "NULL STRING: %lu index, len %lu\n", i, text.textLen);
        }
    }
}
