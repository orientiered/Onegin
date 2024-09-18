#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include "onegin.h"
//#define DEBUG_PRINTS
#include "error_debug.h"
#include "oneginIO.h"

enum status readTextFromFile(const char* fileName, text_t *textInfo) {
    textInfo->textLen = getFileSize(fileName);
    DBG_PRINTF("File size: %lu\n", textInfo->textLen);
    FILE *textFile = fopen(fileName, "rb");
    if (!textFile) {
        fprintf(stderr, "Can't open file\n");
        return ERROR;
    }
    char *data = (char*) calloc(textInfo->textLen + 1, sizeof(char));
    textInfo->text = data;
    DBG_PRINTF("Data pointer: %p\n", data);
    if (!data) {
        fclose(textFile);
        fprintf(stderr, "Can't alloc memory\n");
        return ERROR;
    }

    textInfo->textLen = fread(data, sizeof(char), textInfo->textLen, textFile);
    DBG_PRINTF("File size after fread: %lu\n", textInfo->textLen);
    data[textInfo->textLen] = '\0'; //zeroing symbol after text to ensure safety

    splitString(textInfo); //removing '\r' and replacing '\n' by '\0'; updates textLen and dataLen
    splittedStringToArray(textInfo); //making array with pointers to start of the string

    fclose(textFile);
    return SUCCESS;
}

void writeTextToFile(text_t textInfo, FILE *file) {
    for (size_t rowIndex = 0; rowIndex < textInfo.size; rowIndex++) {
        fputs(textInfo.lines[rowIndex], file);
        fputc('\n', file);
    }
}

/// @brief Splits array by end of line, removes all '\r'
void splitString(text_t *textInfo) {
    unsigned linesCnt = 0;
    char *readPtr = textInfo->text, *writePtr = textInfo->text;
    for(; *readPtr; readPtr++) {
        switch(*readPtr) {
        case '\r':
            break;
        case '\n':
            *writePtr++ = '\0';
            linesCnt++;
            break;
        default:
            *writePtr++ = *readPtr;
            break;
        }
    }
    *writePtr = '\0';
    textInfo->textLen = (size_t)writePtr - (size_t)textInfo->text;
    DBG_PRINTF("File size after removing \\r: %lu\n", textInfo->textLen);

    //checking if last string is empty; we don't count it
    if (*readPtr == 0 && readPtr != textInfo->text && *(readPtr - 1) != '\0')
        linesCnt++;
    textInfo->size = linesCnt;
    DBG_PRINTF("Lines count: %lu\n", textInfo->size);

}

void splittedStringToArray(text_t *textInfo) {
    size_t linesCnt = textInfo->size;
    char **strings = (char**) calloc(textInfo->size+1, sizeof(char *));
    strings[0] = textInfo->text;
    strings[linesCnt] = NULL;

    long long lastPos = -1; //index of last '\0' -> end of line
    for (size_t index = 0, lineIdx = 0;
         lineIdx < linesCnt && (index <= textInfo->textLen);
         index++) {
        if (textInfo->text[index] == '\0') {
            strings[lineIdx] = textInfo->text + lastPos + 1;
            //DBG_PRINTF("Scanned %lu line\n", lineIdx);
            lineIdx++;
            lastPos = (long long)index;
        }
    }
    textInfo->lines = strings;
    DBG_PRINTF("Text array pointer: %p\n", textInfo->lines);

}

size_t getFileSize(const char *fileName) {
    struct stat stBuf = {};
    if (!fileName || stat(fileName, &stBuf) == -1) {
        printf("Can't read file %s\n", fileName);
        return 0;
    }
    return (size_t)stBuf.st_size;
}
