#ifndef ONEGIN_H
#define ONEGIN_H

typedef struct text {
    char **text;        ///< Pointer to array of strings // TODO: genius docs // TODO: naming «lines»?
    size_t textLen;     ///< Number of strings // TODO: variable of type size_t /s (sarcasm, don't do this)
    char *data;         ///< Raw text (stored as long block)
    size_t dataLen;     ///< Length of data (not including \0)
} text_t;

// TODO: use imperative, so «free» instead of «frees»
/// @brief Free text and data pointers
void deleteText(text_t *text);

#endif
