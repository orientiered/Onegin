#include <stdio.h>
#include "error.h"
#include "mystring.h"
#include "utils.h"
#include "argvProcessor.h"

const int ROWS_NUMBER = 1024;
const int LINE_LEN = 512;

enum error readFromFile(const char* fileName, char *text);
void bubbleSort(void *array, int elemSize, int length, int (*cmp)(const void *first, const void *second));
void printText(char *index[], FILE* file);
int strvoidcmp(const void *firstStr, const void *secondStr);

int cmp(const void *firstStr, const void *secondStr) {
    return stralphacmp(*(const char **)firstStr, *(const char**)secondStr);
}



int main(int argc, char *argv[]) {
    argVal_t flags[argsSize] = {};
    initFlags(flags);
    processArgs(flags, argc, argv);

    if (flags[HELP].set) {
        printHelpMessage();
        return 0;
    }

    char text[ROWS_NUMBER][LINE_LEN] = {};
    char *index[ROWS_NUMBER] = {};
    for (size_t i = 0; i < ROWS_NUMBER; i++)
        index[i] = (char*)text + i*LINE_LEN;


    const char *fileName = "onegin-utf-8.txt";
    if (flags[INPUT].set)
        fileName = flags[INPUT].val._string;

    if (readFromFile(fileName, (char*) text) != GOOD_EXIT) {
        fprintf(stderr, "Can't read from file\n");
        return 0;
    }

    bubbleSort(index, sizeof(int*), ROWS_NUMBER, cmp);

    FILE *outFile = stdout;
    if (flags[OUTPUT].set)
        outFile = fopen(flags[OUTPUT].val._string, "w");
    if (!outFile) {
        fprintf(stderr, "Can't open output file\n");
        return 0;
    }
    printText(index, outFile);
    fclose(outFile);
    return 0;
}


enum error readFromFile(const char* fileName, char * text) {
    FILE *textFile = fopen(fileName, "r");
    if (textFile == NULL) return FAIL;

    short correctInput = 1;
    for (size_t lineIndex = 0; lineIndex < ROWS_NUMBER; lineIndex++) {
        fgets(text + LINE_LEN*lineIndex, LINE_LEN, textFile);

        if (feof(textFile)) {
            correctInput = 0;
            break;
        }
    }

    fclose(textFile);
    if (correctInput)
        return GOOD_EXIT;
    else
        return BAD_EXIT;
}


void bubbleSort(void *array, int elemSize, int length, int (*cmp)(const void *first, const void *second)) {
    for (int rightBoundary = length; rightBoundary > 0; rightBoundary--) {
        unsigned char swapFlag = 0; //if we didn't swap anything, array is sorted
        for (int i = 0; i < rightBoundary-1; i++) {
            if (cmp((char*)array + i*elemSize, (char*)array + (i+1)*elemSize) > 0) {
                swapFlag = 1;
                swap((char*)array + i*elemSize, (char*)array + (i+1)*elemSize, elemSize);
            }
        }
        if (!swapFlag) break;
    }
}


int strvoidcmp(const void *firstStr, const void *secondStr) {
    return stralphacmp((const char*) firstStr, (const char*) secondStr);
}


void printText(char *index[], FILE *file) {
    for (size_t rowIndex = 0; rowIndex < ROWS_NUMBER; rowIndex++)
        fprintf(file, "%s", index[rowIndex]);
}
