#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "error.h"
#include "oneginIO.h"

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
