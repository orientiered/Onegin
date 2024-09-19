#ifndef ONEGIN_H
#define ONEGIN_H

typedef struct string {
    char *str;
    size_t size;
} string_t;

typedef struct text {
    string_t *lines;       ///< Array of strings
    string_t *originalLines;
    size_t size;        ///< Number of strings
    char *text;         ///< Raw text (stored as long block)
    size_t textLen;     ///< Length of data (not including \0)
} text_t;

void restoreOriginal(text_t *text);

/// @brief Free text and data pointers
void deleteText(text_t *text);

#endif
