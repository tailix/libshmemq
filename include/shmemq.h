#ifndef SHMEMQ_INCLUDED
#define SHMEMQ_INCLUDED 1

#include <semaphore.h>
#include <stdbool.h>
#include <stddef.h>

#define SHMEMQ_DELETE(shmemq, error_ptr) { \
    shmemq_delete(shmemq, error_ptr);      \
    shmemq = NULL;                         \
}

#define SHMEMQ_NAME_SIZE_MAX ((size_t)255)
#define SHMEMQ_NAME_SLEN_MAX (SHMEMQ_NAME_SIZE_MAX - 1)

// TODO: Resize buffer dynamically.
// #define SHMEMQ_BUFFER_SIZE_MIN (sizeof(struct Shmemq_BufferHeader))
#define SHMEMQ_BUFFER_SIZE_MIN (sizeof(struct ShmemqBufferHeader) + 800)

#define SHMEMQ_FRAME_SIZE ((size_t)16)

#define SHMEMQ_FRAME_DATA_SIZE \
    (SHMEMQ_FRAME_SIZE - sizeof(struct ShmemqFrameHeader))

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ShmemqError {
    SHMEMQ_ERROR_NONE = 0,

    // Invalid values.
    SHMEMQ_ERROR_INVALID_NAME = 1,

    // Bugs in user code.
    SHMEMQ_ERROR_BUG_POP_END_ON_EMPTY_QUEUE = 50,
    SHMEMQ_ERROR_BUG_PUSH_END_ON_FULL_QUEUE = 51,
    SHMEMQ_ERROR_BUG_PUSH_END_OVERFLOW      = 52,

    // Failed system calls.
    SHMEMQ_ERROR_FAILED_MALLOC     = 100,
    SHMEMQ_ERROR_FAILED_SHM_OPEN   = 101,
    SHMEMQ_ERROR_FAILED_FTRUNCATE  = 102,
    SHMEMQ_ERROR_FAILED_MMAP       = 103,
    SHMEMQ_ERROR_FAILED_MUNMAP     = 104,
    SHMEMQ_ERROR_FAILED_CLOSE      = 105,
    SHMEMQ_ERROR_FAILED_SHM_UNLINK = 106,
    SHMEMQ_ERROR_FAILED_SEM_INIT   = 107,
} ShmemqError;

struct ShmemqFrameHeader {
    size_t message_frames_count;
};

typedef struct ShmemqFrame {
    struct ShmemqFrameHeader header;
    unsigned char data[SHMEMQ_FRAME_DATA_SIZE];
} *ShmemqFrame;

struct ShmemqBufferHeader {
    bool is_ready;
    size_t frames_count;
    size_t read_frame_index;
    size_t write_frame_index;
    sem_t read_sem;
    sem_t write_sem;
};

struct ShmemqBuffer {
    struct ShmemqBufferHeader header;
    struct ShmemqFrame frames[];
};

typedef struct Shmemq {
    char name[SHMEMQ_NAME_SIZE_MAX];
    bool is_consumer;
    int shm_id;
    struct ShmemqBuffer *buffer;
} *Shmemq;

Shmemq shmemq_new(const char *name, bool is_consumer, ShmemqError *error_ptr);

void shmemq_init(
    Shmemq shmemq,
    const char *name,
    bool is_consumer,
    ShmemqError *error_ptr
);

void shmemq_delete(Shmemq shmemq, ShmemqError *error_ptr);

void shmemq_finish(Shmemq shmemq, ShmemqError *error_ptr);

ShmemqFrame shmemq_push_start(Shmemq shmemq);

void shmemq_push_end(Shmemq shmemq, size_t data_size, ShmemqError *error_ptr);

ShmemqFrame shmemq_pop_start(Shmemq shmemq);

void shmemq_pop_end(Shmemq shmemq, ShmemqError *error_ptr);

#ifdef __cplusplus
}
#endif

#endif
