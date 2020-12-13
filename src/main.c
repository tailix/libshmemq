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
    const bool is_consumer,
    const size_t size,
    enum Shmemq_Error *const error_ptr
) {
    struct Shmemq *const shmemq = malloc(sizeof(*shmemq));

    if (!shmemq) {
        if (error_ptr) *error_ptr = SHMEMQ_ERROR_FAILED_MALLOC;
        return NULL;
    }

    const enum Shmemq_Error error =
        shmemq_init(shmemq, name, is_consumer, size);

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
    const bool is_consumer,
    size_t size
) {
    if (strlen(name) > SHMEMQ_NAME_SLEN_MAX || name[0] != '/') {
        return SHMEMQ_ERROR_INVALID_NAME;
    }

    for (const char *chr = &name[1]; *chr; ++chr) {
        if (*chr == '/') return SHMEMQ_ERROR_INVALID_NAME;
    }

    strcpy(shmemq->name, name);

    if (size == 0) size = SHMEMQ_BUFFER_FRAMES_COUNT_MIN;

    if (size < SHMEMQ_BUFFER_SIZE_MIN) return SHMEMQ_ERROR_INVALID_SIZE;

    shmemq->is_consumer = is_consumer;

    shmemq->shm_id = shm_open(
        shmemq->name,
        O_CREAT | O_RDWR,
        S_IRUSR | S_IWUSR
    );

    if (shmemq->shm_id == -1) return SHMEMQ_ERROR_FAILED_SHM_OPEN;

    struct stat statbuf;

    if (fstat(shmemq->shm_id, &statbuf) != 0 || statbuf.st_size < 0) {
        shm_unlink(shmemq->name);
        return SHMEMQ_ERROR_FAILED_FSTAT;
    }

    if ((size_t)statbuf.st_size < size) {
        if (ftruncate(shmemq->shm_id, size) != 0) {
            shm_unlink(shmemq->name);
            return SHMEMQ_ERROR_FAILED_FTRUNCATE;
        }
    }

    shmemq->buffer = mmap(
        NULL,
        size,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        shmemq->shm_id,
        0
    );

    if (shmemq->buffer == MAP_FAILED) {
        shm_unlink(shmemq->name);
        return SHMEMQ_ERROR_FAILED_MMAP;
    }

    shmemq->buffer->header.frames_count =
        (size - sizeof(struct Shmemq_BufferHeader)) / SHMEMQ_FRAME_SIZE;

    shmemq->buffer->header.read_frame_index = 0;
    shmemq->buffer->header.write_frame_index = 0;

    return SHMEMQ_ERROR_NONE;
}
