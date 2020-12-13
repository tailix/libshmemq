#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define _POSIX_C_SOURCE 200809L

#include <shmemq.h>

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

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
        return SHMEMQ_ERROR_NAME;
    }

    for (const char *chr = &name[1]; *chr; ++chr) {
        if (*chr == '/') return SHMEMQ_ERROR_NAME;
    }

    strcpy(shmemq->name, name);

    shmemq->is_consumer = size == 0;

    shmemq->shm_id = shm_open(
        shmemq->name,
        O_CREAT | O_RDWR,
        S_IRUSR | S_IWUSR
    );

    if (shmemq->shm_id == -1) return SHMEMQ_ERROR_SHARED_MEMORY;

    struct stat statbuf;

    if (fstat(shmemq->shm_id, &statbuf) != 0) return SHMEMQ_ERROR_FSTAT;

    const size_t min_size =
        shmemq->is_consumer ? sizeof(struct Shmemq_BufferHeader) : size;

    if ((size_t)statbuf.st_size < min_size) {
        if (ftruncate(shmemq->shm_id, min_size) != 0) {
            return SHMEMQ_ERROR_FTRUNCATE;
        }
    }

    if (fstat(shmemq->shm_id, &statbuf) != 0) return SHMEMQ_ERROR_FSTAT;

    if ((size_t)statbuf.st_size < size && !shmemq->is_consumer) {
        if (ftruncate(shmemq->shm_id, size) != 0) return SHMEMQ_ERROR_FTRUNCATE;
    }

    shmemq->buffer = NULL;

    return SHMEMQ_ERROR_NONE;
}
