#include "soloscuro/state.h"
#include <stdlib.h>

extern soloscuro_state_t* soloscuro_state_create() {
    return calloc(1, sizeof(soloscuro_state_t));
}

extern void soloscuro_state_free(soloscuro_state_t *state) {
}
