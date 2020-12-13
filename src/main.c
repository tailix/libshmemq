#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <shmemq.h>

#include <stdlib.h>

struct Shmemq *shmemq_new(
    const char *const name,
    const size_t size,
    enum Shmemq_Error *const error_ptr
) {
    struct Shmemq *const shmemq = malloc(sizeof(*shmemq));

    if (!shmemq) {
        if (error_ptr) *error_ptr = SHMEMQ_ERROR_MALLOC;
        return NULL;
    }

    const enum Shmemq_Error error = shmemq_init(shmemq, name, size);

    if (error_ptr) *error_ptr = error;

    if (error) {
        free(shmemq);
        return NULL;
    }

    return shmemq;
}

enum Shmemq_Error shmemq_init(
    struct Shmemq *const shmemq,
    const char *const name,
    const size_t size
) {
    return SHMEMQ_ERROR_NONE;
}
