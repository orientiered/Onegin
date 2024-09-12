#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "utils.h"
#include "mystring.h"
//#include <string.h>
char *strcpy(char *s, const char *ct) {
    char *s_copy = s;
    while ((*s++=*ct++) != '\0')
        ;
    return s_copy;
}

char *strncpy(char *s, const char *ct, size_t n) {
    char *s_copy = s;
    while (n && (*s++ = *ct++) != '\0')n--;
    if (!n) *s = '\0';
    return s_copy;
}

char *strcat(char *s, const char *ct) {
    char *s_copy = s;
    while (*s) s++;
    while ((*s++ = *ct++) != '\0')
        ;
    return s_copy;
}

char *strncat(char *s, const char *ct, size_t n) {
    char *s_copy = s;
    while (*s) s++;
    while (n && (*s++ = *ct++) != '\0') n--;
    if (!n) *s = '\0';
    return s_copy;
}

int strcmp(const char *firstStr, const char *secondStr) {
    for (; *firstStr && (*firstStr == *secondStr); firstStr++, secondStr++);

    return int(*firstStr) - int(*secondStr);
}


int stralphacmp(const char *firstStr, const char *secondStr) {
    while (*firstStr && *secondStr) {
        firstStr = findAlphabetChar(firstStr);
        secondStr = findAlphabetChar(secondStr);

        if (tolower(*firstStr) != tolower(*secondStr))
            return tolower(*firstStr) - tolower(*secondStr);
        if (*firstStr)  firstStr++;
        if (*secondStr) secondStr++;
    }
    firstStr = findAlphabetChar(firstStr);
    secondStr = findAlphabetChar(secondStr);
    return tolower(*firstStr) - tolower(*secondStr);
}

int stralphacmpBackward(const char *firstStr, const char *secondStr) {
    const char *firstStart = firstStr, *secondStart = secondStr;
    firstStr  += maxINT((long long)strlen(firstStr)  - 1, 0);
    secondStr += maxINT((long long)strlen(secondStr) - 1, 0);

    while (firstStr > firstStart && secondStr > secondStart) {
        firstStr = findAlphabetCharBackward(firstStr, firstStart);
        secondStr = findAlphabetCharBackward(secondStr, secondStart);
        if (tolower(*firstStr) != tolower(*secondStr))
            return tolower(*firstStr) - tolower(*secondStr);
        if (firstStr > firstStart) firstStr--;
        if (secondStr > secondStart) secondStr--;
    }
    firstStr = findAlphabetCharBackward(firstStr, firstStart);
    secondStr = findAlphabetCharBackward(secondStr, secondStart);
    return tolower(*firstStr) - tolower(*secondStr);
}


const char *findAlphabetChar(const char *str) {
    while (*str && !isalpha(*str)) str++;
    return str;
}

const char *findAlphabetCharBackward(const char *str, const char *end) {
    while (str > end && !isalpha(*str)) str--;
    return str;
}

int strncmp(const char *s, const char *t, size_t n) {
    for (;n && *s && (*s == *t); s++, t++, n--);

    if (*s != *t)
        return int(*s) - *t;
    return 0;
}

char *strchr(char *s, char c) {
    while (*s != c && *s) s++;
    if (*s == c)
        return s;
    else
        return NULL;
}

char *strrchr(char *s, char c) {
    const char *start = s;
    while (*s) s++;
    while (*s != c && s > start) --s;
    if (*s == c)
        return s;
    else
        return NULL;
}

const char* strstr(const char* text, const char* str) {
    int textSize = (int) strlen(text);
    int strSize =  (int) strlen(str);

    if (strSize > textSize) return NULL;

    int* prefix = (int*) calloc(strSize, sizeof(int));
    prefix[0] = 0;
    for (int i = 1; i < strSize; i++) {
        int curLen = prefix[i-1];
        while (curLen > 0 && str[i] != str[curLen])
            curLen = prefix[curLen-1];
        if (str[i] == str[curLen])
            curLen++;
        prefix[i] = curLen;
    }

    int curLen = 0;
    for (int i = 0; i < textSize; i++) {
        while (curLen > 0 && text[i] != str[curLen])
            curLen = prefix[curLen-1];

        if (text[i] == str[curLen])
            curLen++;

        if (curLen == strSize) {
            free(prefix);
            return text + i - strSize + 1;
        }
    }

    free(prefix);
    return NULL;
}

size_t strlen(const char *s) {
    size_t len = 0;
    while (*s++) len++;
    return len;
}


