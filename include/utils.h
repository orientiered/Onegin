/// @file
/// @brief
#ifndef UTILS_H
#define UTILS_H

void swap(void* a, void* b, size_t len);
void swapByByte(void* a, void* b, size_t len);

void memcpy(void* copyTo, void* copyFrom, size_t length);
void memcpyByByte(void* copyTo, void* copyFrom, size_t length);

#endif
