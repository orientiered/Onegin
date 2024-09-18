#ifndef ONEGIN_H
#define ONEGIN_H

typedef struct text {
    char **lines;       ///< Array of strings
    size_t size;        ///< Number of strings
    char *text;         ///< Raw text (stored as long block)
    size_t textLen;     ///< Length of data (not including \0)
} text_t;

/// @brief Free text and data pointers
void deleteText(text_t *text);

#endif
