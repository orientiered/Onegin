#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "onegin.h"
//#define DEBUG_PRINTS
#include "error_debug.h"
#include "oneginIO.h"

enum error readTextFromFile(const char* fileName, text_t *textInfo) {
    textInfo->dataLen = getFileSize(fileName);
    DBG_PRINTF("File size: %lu\n", textInfo->dataLen);
    FILE *textFile = fopen(fileName, "rb");
    if (!textFile) return FAIL;

    char *data = (char*) calloc(textInfo->dataLen + 1, sizeof(char));
    textInfo->data = data;
    DBG_PRINTF("Data pointer: %p\n", data);
    if (!data) {
        fclose(textFile);
        return BAD_EXIT;
    }

    textInfo->dataLen = fread(data, sizeof(char), textInfo->dataLen, textFile);
    DBG_PRINTF("File size after fread: %lu\n", textInfo->dataLen);
    data[textInfo->dataLen] = '\0'; //zeroing symbol after text to ensure safety

    splitString(textInfo); //removing '\r' and replacing '\n' by '\0'; updates textLen and dataLen
    splittedStringToArray(textInfo); //making array with pointers to start of the string

    fclose(textFile);
    return GOOD_EXIT;
}

void writeTextToFile(text_t textInfo, FILE *file) {
    for (size_t rowIndex = 0; rowIndex < textInfo.textLen; rowIndex++) {
        fputs(textInfo.text[rowIndex], file);
        fputc('\n', file);
    }
}

/// @brief Splits array by end of line, removes all '\r'
void splitString(text_t *textInfo) {
    unsigned linesCnt = 0;
    char *readPtr = textInfo->data, *writePtr = textInfo->data;
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
    textInfo->dataLen = (size_t)writePtr - (size_t)textInfo->data;
    DBG_PRINTF("File size after removing \\r: %lu\n", textInfo->dataLen);

    //checking if last string is empty; we don't count it
    if (*readPtr == 0 && readPtr != textInfo->data && *(readPtr - 1) != '\0')
        linesCnt++;
    textInfo->textLen = linesCnt;
    DBG_PRINTF("Lines count: %lu\n", textInfo->textLen);

}

void splittedStringToArray(text_t *textInfo) {
    size_t linesCnt = textInfo->textLen;
    char **strings = (char**) calloc(textInfo->textLen+1, sizeof(char *));
    strings[0] = textInfo->data;
    strings[linesCnt] = NULL;

    long long lastPos = -1; //index of last '\0' -> end of line
    for (size_t index = 0, lineIdx = 0;
         lineIdx < linesCnt && (index <= textInfo->dataLen);
         index++) {
        if (textInfo->data[index] == '\0') {
            strings[lineIdx] = textInfo->data + lastPos + 1;
            //DBG_PRINTF("Scanned %lu line\n", lineIdx);
            lineIdx++;
            lastPos = (long long)index;
        }
    }
    textInfo->text = strings;
    DBG_PRINTF("Text array pointer: %p\n", textInfo->text);

}

size_t getFileSize(const char *fileName) {
    struct stat stBuf = {};
    if (!fileName || stat(fileName, &stBuf) == -1) {
        printf("Can't read file %s\n", fileName);
        return 0;
    }
    return (size_t)stBuf.st_size;
}
