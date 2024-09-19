#include <stdio.h>
#include <stdlib.h>
#define DEBUG_PRINTS
#include "error_debug.h"
#include "utils.h"
#include "mystring.h"
#include "argvProcessor.h"
#include "sorters.h"
#include "onegin.h"
#include "oneginIO.h"
#include "metrics.h"

#include "main.h"


int main(int argc, const char *argv[]) {
// TODO: log file status time position
// no bufferization
    FlagsHolder_t flags = {};
    PROPAGATE_ERROR(processArgs(flagsDescriptions, &flags, argc, argv));
    if (isFlagSet(flags, "-h")) {
        printHelpMessage(flagsDescriptions);
        deleteFlags(&flags);
        return 0;
    }

    text_t onegin = {};
    const char *fileName = "OneginText.txt"; // TODO: get_argument_or_default
    if (isFlagSet(flags, "-i"))
        fileName = getFlagValue(flags, "-i").string_;
    USER_ERROR(readTextFromFile(fileName, &onegin), deleteFlags(&flags));
    FILE *outFile = stdout;
    USER_ERROR(getOutputFile(flags, &outFile), {deleteFlags(&flags); deleteText(&onegin);});

    cmpFuncPtr_t cmpFuncs[] = {stringArrAlphaCmp, stringArrAlphaCmpBackwardFast};
    sortFuncPtr_t sortFunc = chooseSortFunction(getFlagValue(flags, "-s").string_); //quickSort by default

    for (size_t cmpIndex = 0; cmpIndex < ARRAY_SIZE(cmpFuncs); cmpIndex++) {
        sortFunc(onegin.lines, onegin.size, sizeof(string_t), cmpFuncs[cmpIndex]);
        #ifndef NDEBUG
        if (checkIsSorted(onegin, cmpFuncs[cmpIndex]))
            printf("Sort doesn't work\n");
        #endif
        writeTextToFile(onegin.lines, onegin.size, outFile); //forward sorting
        DBG_PRINTF("%lu write\n", cmpIndex + 1);
    }

    writeTextToFile(onegin.originalLines, onegin.size, outFile);

    testSortingFunction(flags, onegin, sortFunc, cmpFuncs[0]); //only with -t flag
    closeAndFreeAll(&onegin, &flags, outFile);

    return 0;
}

int checkIsSorted(text_t text, cmpFuncPtr_t cmp) {
    int notSorted = 0;
    for (size_t i = 0; i < text.size-1; i++) {
        if (cmp(text.lines + i, text.lines + i + 1) > 0) {
            DBG_PRINTF("i = %lu\n%s\n%s\n", i, (text.lines + i)->str, (text.lines + i + 1)->str);
            notSorted = 1;
        }
    }
    return notSorted;
}

void testSortingFunction(FlagsHolder_t flags, text_t onegin, sortFuncPtr_t sortFunc, cmpFuncPtr_t cmp) {
    const int TEST_NUMBER = 50;
    if (isFlagSet(flags, "-t")) {
        sortTimeTest(TEST_NUMBER, onegin, sortFunc, cmp);
        if (isFlagSet(flags, "-g"))
            plotSortTimeGraph();
    }
}

enum status getOutputFile(FlagsHolder_t flags, FILE **outFile) {
    if (isFlagSet(flags, "-o"))
        *outFile = fopen(getFlagValue(flags, "-o").string_, "wb");
    if (!*outFile) {
        fprintf(stderr, "Can't open output file\n");
        return ERROR;
    }
    return SUCCESS;
}

void closeAndFreeAll(text_t *onegin, FlagsHolder_t *flags, FILE* outFile) {
    deleteText(onegin);
    deleteFlags(flags);
    if (outFile && outFile != stdout)
        fclose(outFile);
}


// void test() {
//     unsigned long long *data = (unsigned long long*) calloc(1000, sizeof(unsigned long long));
//     for (int i = 0; i < 1000; i++) data[i] = abs((rand() % 1000) * 1000);
//     quickSort(data, 1000, sizeof(unsigned long long), ullCmp);
//     for (size_t i = 0; i < 1000 - 1; i++) {
//         if (ullCmp(&data[i], &data[i+1]) > 0) {
//             DBG_PRINTF("i = %lu\n%llu\n%llu\n", i, data[i], data[i+1]);
//         }
//     }
//     free(data);
// }
