#ifndef MY_STRING_H
#define MY_STRING_H

char *strcpy(char *s, const char *ct);
char *strncpy(char *s, const char *ct, size_t n);

char *strcat(char *s, const char *ct);
char *strncat(char *s, const char *ct, size_t n);

int strcmp(const char *s, const char *t);
int strncmp(const char *s, const char *t, size_t n);

char *strchr(char *s, char c);
char *strrchr(char *s, char c);

const char *strstr(const char *s, const char *t );

char *strtok(char *s, const char *tok);

size_t strlen(const char *s);

#endif
