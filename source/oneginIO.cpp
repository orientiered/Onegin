#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include "onegin.h"
//#define DEBUG_PRINTS
#include "error_debug.h"
#include "logger.h"
#include "mystring.h"
#include "utils.h"
#include "oneginIO.h"

enum status openFile(FILE **file, const char *fileName, const char *mode) {
    MY_ASSERT(file, abort());
    MY_ASSERT(fileName, abort());
    MY_ASSERT(mode, abort());

    *file = fopen(fileName, mode);
    if (!file) {
        logPrint(L_ZERO, 1, "Can't open file %s\n", fileName);
        return ERROR;
    }
    return SUCCESS;
}

enum status readTextFromFile(const char* fileName, text_t *textInfo) {
    USER_ERROR(getFileSize(fileName, &textInfo->textLen), ;);
    DBG_PRINTF("File size: %lu\n", textInfo->textLen);

    FILE *textFile = NULL;
    USER_ERROR(openFile(&textFile, fileName, "rb"), ;);

    char *data = (char*) calloc(textInfo->textLen + 1, sizeof(char));
    textInfo->text = data;
    // TODO: function to print error:
    // source, function, version, custom error message
    // -v -> verbose logs -^ for fun
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

void writeTextToFile(string_t *lines, size_t size, FILE *file) {
    for (size_t rowIndex = 0; rowIndex < size; rowIndex++) {
        fputs(lines[rowIndex].str, file);
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
    string_t *strings = (string_t *) calloc(textInfo->size, sizeof(string_t));;
    strings[0].str = textInfo->text;

    long long lastPos = -1; //index of last '\0' -> end of line
    // TODO: ssize_t <- POSIX only
    for (size_t index = 0, lineIdx = 0;
         lineIdx < linesCnt && (index <= textInfo->textLen);
         index++) {
        if (textInfo->text[index] == '\0') {
            strings[lineIdx].str = textInfo->text + lastPos + 1;
            strings[lineIdx].size = index - (lastPos + 1);
            //DBG_PRINTF("Scanned %lu line\n", lineIdx);
            lineIdx++;
            lastPos = (long long)index;
        }
    }

    textInfo->originalLines = strings;
    // for (size_t idx = 0; idx < textInfo->size; idx++)
        // textInfo->originalLines[idx].size = strlen(strings[idx].str);

    textInfo->lines = (string_t *) calloc(textInfo->size, sizeof(string_t));
    //TODO: check this calloc
    //TODO: wrapper on calloc to log memory allocations and check for leaks
    DBG_PRINTF("Lines pointers: %p %p\n", textInfo->originalLines, textInfo->lines);

    memcpy(textInfo->lines, textInfo->originalLines, textInfo->size * sizeof(string_t));
    DBG_PRINTF("Text array pointer: %p\n", textInfo->lines);
}

enum status getFileSize(const char *fileName, size_t *size) {
    struct stat stBuf = {};
    if (!fileName || stat(fileName, &stBuf) == -1) {
        // logPrint(L_ZERO, 1, "Can't read file %s\n", fileName);
        logPrint(L_ZERO, 1, "Can't read file %s\n", fileName);
        return ERROR;
    }
    *size = (size_t)stBuf.st_size;
    return SUCCESS;
}
