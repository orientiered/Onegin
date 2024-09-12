#ifndef ONEGIN_H
#define ONEGIN_H

typedef struct text {
    char **text;        ///< Pointer to array of strings
    size_t textLen;     ///< Number of strings
    char *data;         ///< Raw text (stored as long block)
    size_t dataLen;     ///< Length of data (not including \0)
} text_t;

/// @brief Frees text and data pointers
void deleteText(text_t *text);

#endif
