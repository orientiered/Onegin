#include <stdlib.h>
#include "onegin.h"

void deleteText(text_t *text) {
    free(text->data); text->data = NULL;
    free(text->text); text->text = NULL;
}
