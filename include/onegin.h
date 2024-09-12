#ifndef ONEGIN_H
#define ONEGIN_H

typedef struct text {
    char **text;
    size_t textLen;
    char *data;
    size_t dataLen;
} text_t;

void deleteText(text_t *text);

#endif
