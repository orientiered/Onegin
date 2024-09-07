#include <stdio.h>
#include "mystring.h"
#include "utils.h"

const int ROWS_NUMBER = 1024;
const int LINE_LEN = 512;

int readFromFile(const char* fileName, char *text);
void bubbleSort(void *array, int elemSize, int length, int (*cmp)(const void *first, const void *second));
void printText(int *index, char *text);
// int strindexcmp(const int firstIdx, const int secondIdx);
int strvoidcmp(const void *firstStr, const void *secondStr);

int main() {
    char text[ROWS_NUMBER][LINE_LEN] = {};
    int index[ROWS_NUMBER] = {};
    for (size_t i = 0; i < ROWS_NUMBER; i++)
        index[i] = i;

    if (readFromFile("Romeo and Juliet.txt", (char*) text)) {
        fprintf(stderr, "Can't read from file\n");
        return 0;
    }
    bubbleSort((char*)text, LINE_LEN, ROWS_NUMBER, strvoidcmp);

    printText(index, (char *) text);
}


int readFromFile(const char* fileName, char * text) {
    FILE *textFile = fopen(fileName, "r");
    if (textFile == NULL) return 1;

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
        return 0;
    else
        return 1;
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

void printText(int *index, char *text) {
    for (size_t rowIndex = 0; rowIndex < ROWS_NUMBER; rowIndex++)
        printf("%s", text + index[rowIndex]*LINE_LEN);
}
