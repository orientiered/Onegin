#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "error.h"
#include "mystring.h"
#include "utils.h"
#include "argvProcessor.h"
#include "sorters.h"



enum error readFromFile(const char* fileName, char ***text, size_t *length);
void printText(char *index[], size_t length, FILE* file);


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
    char *fullText = textStrings[0];

    bubbleSort(textStrings, sizeof(char*), stringsCnt, stringArrayCmp);

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


enum error readFromFile(const char* fileName, char ***text, size_t *length) {
    FILE *textFile = fopen(fileName, "r");
    if (textFile == NULL) return FAIL;

    struct stat stBuf = {};
    fstat(fileno(textFile), &stBuf);
    char *data = (char*) calloc((size_t)stBuf.st_size, 1);
    if (!data) {
        fclose(textFile);
        return BAD_EXIT;
    }


    // fread(data, stBuf.st_size, sizeof(char), textFile);
    unsigned linesCnt = 0, skippedCR = 0;
    for (size_t index = 0; (index + skippedCR) < (size_t) stBuf.st_size; index++) {
        data[index] = fgetc(textFile);
        if (data[index] == '\n') {
            if (index > 0 && data[index-1] == '\r')
                skippedCR++, index--;
            data[index] = '\0';
            linesCnt++;
        }
    }

    char **strings = (char**) calloc(linesCnt, sizeof(char *));
    strings[0] = data;
    int lastPos = -1;
    for (size_t index = 0, lineIdx = 0; lineIdx < linesCnt && (index < ((size_t)stBuf.st_size + skippedCR)); index++) {
        if (data[index] == '\0') {
            strings[lineIdx] = data + lastPos + 1;
            lineIdx++;
            lastPos = index;
        }
    }
    *text = strings;
    *length = linesCnt;
    fclose(textFile);
    return GOOD_EXIT;
}

void printText(char *index[], size_t length, FILE *file) {
    for (size_t rowIndex = 0; rowIndex < length; rowIndex++)
        fprintf(file, "%s\n", index[rowIndex]);
}
