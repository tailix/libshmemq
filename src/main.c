#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <shmemq.h>

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>

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
    if (strlen(name) > SHMEMQ_NAME_SLEN_MAX || name[0] != '/') {
        return SHMEMQ_ERROR_INVALID_NAME;
    }

    for (const char *chr = &name[1]; *chr; ++chr) {
        if (*chr == '/') return SHMEMQ_ERROR_INVALID_NAME;
    }

    strcpy(shmemq->name, name);

    shmemq->is_consumer = size == 0;

    shmemq->shm_id = shm_open(
        shmemq->name,
        O_CREAT | O_RDWR,
        S_IRUSR | S_IWUSR
    );

    if (shmemq->shm_id == -1) return SHMEMQ_ERROR_SHARED_MEMORY;

    shmemq->buffer = NULL;

    return SHMEMQ_ERROR_NONE;
}
