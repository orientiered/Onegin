/// @file
/// @brief
#ifndef UTILS_H
#define UTILS_H

typedef struct doublePair {
    double first;
    double second;
} doublePair_t;

typedef struct llPair {
    long long first;
    long long second;
} llPair_t;

long long maxINT(long long a, long long b);
long long minINT(long long a, long long b);

void swap(void* a, void* b, size_t len);
void swapByByte(void* a, void* b, size_t len);

void memcpy(void* copyTo, void* copyFrom, size_t length);
void memcpyByByte(void* copyTo, void* copyFrom, size_t length);

doublePair_t runningSTD(double value, int getResult);

#endif
