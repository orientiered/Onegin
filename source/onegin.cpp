#include <stdlib.h>
#include "utils.h"
#include "onegin.h"

void restoreOriginal(text_t *text) {
    memcpy(text->lines, text->originalLines, text->size * sizeof(char*));
}

void deleteText(text_t *text) {
    FREE(text->text);
    FREE(text->originalLines);
    FREE(text->lines);
}
