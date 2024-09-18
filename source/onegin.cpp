#include <stdlib.h>
#include "onegin.h"

void deleteText(text_t *text) {
    free(text->text); text->text = NULL;
    free(text->lines); text->lines = NULL;
}
