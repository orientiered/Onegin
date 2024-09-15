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
void test();

// TODO: make main look less like yours and more like this:
//
// parsed_args = parse_args(args, argc, argv);
// if (fignya...(parsed_args))
//     return EXIT_FAILURE;
//
//  input_file = get_argument_or_default(parsed, args, "--input", "onegin.txt");
// output_file = get_argument(parsed, args, "--output");
//
// propagate_error(text = read_text(input_file))
//
// sort(text, lexicographical)
// propagate_error(print(text, output_file))
//
// sort(text, lexicographical_reversed)
// propagate_error(print(text, output_file))
//
// restore_original(text)
// propagate_error(print(text, output_file))


int main(int argc, char *argv[]) { //TODO: const char *argv[]
    test();
    argVal_t flags[argsSize] = {};
    initFlags(flags);
    processArgs(flags, argc, argv);

    if (flags[HELP].set) {
        printHelpMessage();
        return 0;
    }

    text_t onegin = {};

    const char *fileName = "OneginText.txt"; // TODO: get_argument_or_default
    if (flags[INPUT].set)
        fileName = flags[INPUT].val._string;

    if (readTextFromFile(fileName, &onegin) != GOOD_EXIT) { // TODO: is_success(...)?
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
    cmpFuncPtr_t cmpFuncs[] = {stringArrCmpBackward, stringArrAlphaCmp, ullCmp};

    if (flags[SORT_ALG].set) {
        sortFunc = chooseSortFunction(flags[SORT_ALG].val._string);
    }

    pthread_t plotThread = 0;
    if (flags[SORT_TIME].set) {
        doublePair_t averageTime = sortTimeTest(50, onegin, sortFunc, cmpFuncs[0], &plotThread);
        if (averageTime.first > 40000) //>40ms // TODO: this is a bit weird?
            printf("Average sorting time is %.1f+-%.1f ms\n", averageTime.first / 1000, averageTime.second / 1000);
        else
            printf("Average sorting time is %.3f+-%.3f ms\n", averageTime.first / 1000, averageTime.second / 1000);
    }

    // TODO: ARRAY_SIZE macro? ARRAY_SIZE(array) (sizeof(array) / sizeof(*array))
    for (size_t cmpIndex = 0; cmpIndex < sizeof(cmpFuncs)/sizeof(cmpFuncPtr_t); cmpIndex++) {
        sortFunc(onegin.text, onegin.textLen, sizeof(char*), cmpFuncs[cmpIndex]);
        #ifndef NDEBUG
        if (checkIsSorted(onegin, cmpFuncs[cmpIndex]))
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
    int notSorted = 0;
    for (size_t i = 0; i < textInfo.textLen-1; i++) {
        if (cmp(textInfo.text + i, textInfo.text + i + 1) > 0) {
            DBG_PRINTF("i = %u\n%s\n%s\n", i, *(textInfo.text + i), *(textInfo.text + i + 1));
            notSorted = 1;
        }
    }
    return notSorted;
}

void checkNULLStrings(text_t text) {
    //debug function to detect broken text
    for (size_t i = 0; i < text.textLen; i++) {
        if (!text.text[i]) {
            fprintf(stderr, "NULL STRING: %lu index, len %lu\n", i, text.textLen);
        }
    }
}

void test() {
    unsigned long long *data = (unsigned long long*) calloc(1000, sizeof(unsigned long long));
    for (int i = 0; i < 1000; i++) data[i] = (rand() % 1000) * 1000;
    quickSort(data, 1000, sizeof(unsigned long long), ullCmp);
    for (size_t i = 0; i < 1000 - 1; i++) {
        if (ullCmp(&data[i], &data[i+1]) > 0) {
            DBG_PRINTF("i = %u\n%u\n%u\n", i, data[i], data[i+1]);
        }
    }
    free(data);
}
