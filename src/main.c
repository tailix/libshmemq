#define _POSIX_C_SOURCE 200809L

#include <shmemq.h>

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

void shmemq_delete(const Shmemq shmemq, ShmemqError *const error_ptr)
{
    shmemq_finish(shmemq, error_ptr);
    free(shmemq);
}

void shmemq_finish(const Shmemq shmemq, ShmemqError *const error_ptr)
{
    const size_t size =
        sizeof(struct ShmemqBufferHeader) +
        SHMEMQ_FRAME_SIZE * shmemq->buffer->header.frames_count;

    if (munmap(shmemq->buffer, size) != 0) {
        if (error_ptr) *error_ptr = SHMEMQ_ERROR_FAILED_MUNMAP;
        return;
    }

    shmemq->buffer = NULL;

    if (close(shmemq->shm_id) != 0) {
        if (error_ptr) *error_ptr = SHMEMQ_ERROR_FAILED_CLOSE;
        return;
    }

    shmemq->shm_id = -1;

    if (shmemq->is_consumer && shm_unlink(shmemq->name) != 0) {
        if (error_ptr) *error_ptr = SHMEMQ_ERROR_FAILED_SHM_UNLINK;
        return;
    }

    if (error_ptr) *error_ptr = SHMEMQ_ERROR_NONE;
}

Shmemq shmemq_new(
    const char *const name,
    const bool is_consumer,
    ShmemqError *const error_ptr
) {
    const Shmemq shmemq = malloc(sizeof(*shmemq));

    if (!shmemq) {
        if (error_ptr) *error_ptr = SHMEMQ_ERROR_FAILED_MALLOC;
        return NULL;
    }

    ShmemqError error;
    shmemq_init(shmemq, name, is_consumer, &error);

    if (error_ptr) *error_ptr = error;

    if (error) {
        free(shmemq);
        return NULL;
    }

    if (error_ptr) *error_ptr = SHMEMQ_ERROR_NONE;
    return shmemq;
}

void shmemq_init(
    const Shmemq shmemq,
    const char *const name,
    const bool is_consumer,
    ShmemqError *const error_ptr
) {
    if (strlen(name) > SHMEMQ_NAME_SLEN_MAX || name[0] != '/') {
        *error_ptr = SHMEMQ_ERROR_INVALID_NAME;
        return;
    }

    for (const char *chr = &name[1]; *chr; ++chr) {
        if (*chr == '/') {
            if (error_ptr) *error_ptr = SHMEMQ_ERROR_INVALID_NAME;
            return;
        }
    }

    strcpy(shmemq->name, name);

    shmemq->is_consumer = !!is_consumer;

    shmemq->shm_id = shm_open(
        shmemq->name,
        O_RDWR | (shmemq->is_consumer ? O_CREAT | O_EXCL : 0),
        S_IRUSR | S_IWUSR
    );

    if (shmemq->shm_id == -1) {
        if (error_ptr) *error_ptr = SHMEMQ_ERROR_FAILED_SHM_OPEN;
        return;
    }

    if (shmemq->is_consumer) {
        if (ftruncate(shmemq->shm_id, SHMEMQ_BUFFER_SIZE_MIN) != 0) {
            shm_unlink(shmemq->name);
            if (error_ptr) *error_ptr = SHMEMQ_ERROR_FAILED_FTRUNCATE;
            return;
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
        if (error_ptr) *error_ptr = SHMEMQ_ERROR_FAILED_MMAP;
        return;
    }

    // We don't really need this condition, but it is useful in tests
    // for platform sanity check and maybe we can use it to prevent race
    // condition.
    if (shmemq->is_consumer) {
        shmemq->buffer->header.is_ready = false;

        // TODO: Resize buffer dynamically.
        // shmemq->buffer->header.frames_count = 0;
        shmemq->buffer->header.frames_count = 100;

        shmemq->buffer->header.read_frame_index = 0;
        shmemq->buffer->header.write_frame_index = 0;
    }

    if (error_ptr) *error_ptr = SHMEMQ_ERROR_NONE;
}

ShmemqFrame shmemq_push_start(const Shmemq shmemq)
{
    return &shmemq->buffer->frames[shmemq->buffer->header.write_frame_index];
}
