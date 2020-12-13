#define _POSIX_C_SOURCE 200809L

#include <shmemq.h>

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

enum Shmemq_Error shmemq_delete(struct Shmemq *shmemq)
{
    const enum Shmemq_Error error = shmemq_finish(shmemq);
    free(shmemq);
    return error;
}

enum Shmemq_Error shmemq_finish(struct Shmemq *shmemq)
{
    const size_t size =
        sizeof(struct Shmemq_BufferHeader) +
        SHMEMQ_FRAME_SIZE * shmemq->buffer->header.frames_count;

    if (munmap(shmemq->buffer, size) != 0) return SHMEMQ_ERROR_FAILED_MUNMAP;
    shmemq->buffer = NULL;

    if (close(shmemq->shm_id) != 0) return SHMEMQ_ERROR_FAILED_CLOSE;
    shmemq->shm_id = -1;

    if (shmemq->is_consumer && shm_unlink(shmemq->name) != 0) {
        return SHMEMQ_ERROR_FAILED_SHM_UNLINK;
    }

    return SHMEMQ_ERROR_NONE;
}

struct Shmemq *shmemq_new(
    const char *const name,
    const bool is_consumer,
    enum Shmemq_Error *const error_ptr
) {
    struct Shmemq *const shmemq = malloc(sizeof(*shmemq));

    if (!shmemq) {
        if (error_ptr) *error_ptr = SHMEMQ_ERROR_FAILED_MALLOC;
        return NULL;
    }

    const enum Shmemq_Error error = shmemq_init(shmemq, name, is_consumer);

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
    const bool is_consumer
) {
    if (strlen(name) > SHMEMQ_NAME_SLEN_MAX || name[0] != '/') {
        return SHMEMQ_ERROR_INVALID_NAME;
    }

    for (const char *chr = &name[1]; *chr; ++chr) {
        if (*chr == '/') return SHMEMQ_ERROR_INVALID_NAME;
    }

    strcpy(shmemq->name, name);

    shmemq->is_consumer = !!is_consumer;

    shmemq->shm_id = shm_open(
        shmemq->name,
        O_RDWR | (shmemq->is_consumer ? O_CREAT | O_EXCL : 0),
        S_IRUSR | S_IWUSR
    );

    if (shmemq->shm_id == -1) return SHMEMQ_ERROR_FAILED_SHM_OPEN;

    if (shmemq->is_consumer) {
        if (ftruncate(shmemq->shm_id, SHMEMQ_BUFFER_SIZE_MIN) != 0) {
            shm_unlink(shmemq->name);
            return SHMEMQ_ERROR_FAILED_FTRUNCATE;
        }
    }

    shmemq->buffer = mmap(
        NULL,
        SHMEMQ_BUFFER_SIZE_MIN,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        shmemq->shm_id,
        0
    );

    if (shmemq->buffer == MAP_FAILED) {
        shm_unlink(shmemq->name);
        return SHMEMQ_ERROR_FAILED_MMAP;
    }

    // We don't really need this condition, but it is useful in tests
    // for platform sanity check and maybe we can use it to prevent race
    // condition.
    if (shmemq->is_consumer) {
        shmemq->buffer->header.is_ready = false;
        shmemq->buffer->header.frames_count = 0;
        shmemq->buffer->header.read_frame_index = 0;
        shmemq->buffer->header.write_frame_index = 0;
    }

    return SHMEMQ_ERROR_NONE;
}
