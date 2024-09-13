#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
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
doublePair_t sortTimeTest(unsigned testNumber, text_t onegin, sortFuncPtr_t sortFunc, cmpFuncPtr_t cmp, pthread_t *plotThread);
void percentageBar(unsigned value, unsigned maxValue, unsigned points, long long timePassed);
void *plotGraph(void *arg);

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

doublePair_t sortTimeTest(unsigned testNumber, text_t onegin, sortFuncPtr_t sortFunc, cmpFuncPtr_t cmp, pthread_t *plotThread) {
    FILE *graph = fopen("statistics/sortGraph.py", "wb");
    fprintf(graph,  "import matplotlib.pyplot as plt\n"
                    "import numpy as np\n"
                    "data = []\n");

    clock_t startTime = 0, endTime = 0;
    clock_t totalTime = 0;
    char **textCopy = (char**) calloc(onegin.textLen, sizeof(char*));
    memcpy(textCopy, onegin.text, onegin.textLen * sizeof(char*));

    runningSTD(0, -1); //reseting runningSTD
    printf("Testing:\n");
    for (unsigned test = 0; test < testNumber; test++) {
        percentageBar(test, testNumber, 15, totalTime);
        startTime = clock();
        sortFunc(onegin.text, sizeof(char*), onegin.textLen, cmp);
        endTime = clock();
        runningSTD(endTime - startTime, 0);
        fprintf(graph, "data.append(%f)\n", double(endTime-startTime) / CLOCKS_PER_SEC * 1000);
        DBG_PRINTF(">>sortTime %3.3f\n", double(endTime - startTime) / CLOCKS_PER_SEC * 1000);
        memcpy(onegin.text, textCopy, onegin.textLen * sizeof(char*));
        totalTime += clock() - startTime;
        percentageBar(test+1, testNumber, 15, totalTime);
    }
    //returning average time in microseconds
    doublePair_t result = runningSTD(0, 1);
    result.first  *= 1000.0*1000/CLOCKS_PER_SEC;
    result.second *= 1000.0*1000/CLOCKS_PER_SEC;
    free(textCopy);

    fprintf(graph,  "plt.plot(data)\n"
                    "plt.xlabel(\"Test number\")\n"
                    "plt.ylabel(\"Test time, ms\")\n"
                    "plt.grid()\n"
                    //"plt.gca().set_ylim(bottom=0)\n"
                    "plt.show()\n");
    fclose(graph);
    pthread_create(plotThread, NULL, plotGraph, NULL);
    printf("\n\n");
    printf("Testing took %3.2f s\n", double(totalTime) / CLOCKS_PER_SEC);
    return result;
}

void percentageBar(unsigned value, unsigned maxValue, unsigned points, long long timePassed) {
    //draw nice progress bar like
    //[###|-----] 20.0% Remaining time: 20.4 s
    printf("\r[");
    for (unsigned i = 0; i < points; i++) {
        double pointFill = double(value) / maxValue - double(i) / points;
        if (pointFill > 0)
            printf("#");
        else if (pointFill > -0.5 / points)
            printf("|");
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
    //debug function to detect broken text
    for (size_t i = 0; i < text.textLen; i++) {
        if (!text.text[i]) {
            fprintf(stderr, "NULL STRING: %lu index, len %lu\n", i, text.textLen);
        }
    }
}

void *plotGraph(void *arg) {
    system("python3 statistics/sortGraph.py");
    return NULL;
}
