#include <stdio.h>
#include "mystring.h"
#include "utils.h"

const int ROWS_NUMBER = 2000;
const int LINE_LEN = 256;

int readFromFile(const char* fileName, char * text);
void bubbleSort(char *text);
void printText(char *text);

int main() {
    char text[ROWS_NUMBER][LINE_LEN] = {};
    if (readFromFile("onegin.txt", (char*) text)) {
        fprintf(stderr, "Can't read from file\n");
        return 0;
    }

    bubbleSort((char*) text);

    printText((char*) text);
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


void bubbleSort(char *text) {
    for (int rightBoundary = ROWS_NUMBER; rightBoundary > 0; rightBoundary--) {
        unsigned char swapFlag = 0; //if we didn't swap anything, array is sorted
        for (int i = 0; i < rightBoundary-1; i++) {
            if (strcmp(text + i*LINE_LEN, text + (i+1)*LINE_LEN) > 0) {
                swapFlag = 1;
                swap(text + i*LINE_LEN, text + (i+1)*LINE_LEN, LINE_LEN);
            }
        }
        if (!swapFlag) break;
    }
}


void printText(char *text) {
    for (size_t rowIndex = 0; rowIndex < ROWS_NUMBER; rowIndex++)
        printf("%s", text + rowIndex*LINE_LEN);
}
