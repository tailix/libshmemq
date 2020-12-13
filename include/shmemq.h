#ifndef SHMEMQ_INCLUDED
#define SHMEMQ_INCLUDED 1

#include <stdbool.h>
#include <stddef.h>

#define SHMEMQ_NAME_SIZE_MAX ((size_t)255)
#define SHMEMQ_NAME_SLEN_MAX (SHMEMQ_NAME_SIZE_MAX - 1)

// TODO: Resize buffer dynamically.
// #define SHMEMQ_BUFFER_SIZE_MIN (sizeof(struct Shmemq_BufferHeader))
#define SHMEMQ_BUFFER_SIZE_MIN (sizeof(struct ShmemqBufferHeader) + 800)

#define SHMEMQ_FRAME_SIZE ((size_t)8)

#define SHMEMQ_FRAME_DATA_SIZE \
    (SHMEMQ_FRAME_SIZE - sizeof(struct ShmemqFrameHeader))

#ifdef __cplusplus
extern "C" {
#endif

enum ShmemqError {
    SHMEMQ_ERROR_NONE = 0,

    SHMEMQ_ERROR_INVALID_NAME = 1,

    SHMEMQ_ERROR_FAILED_MALLOC     = 255,
    SHMEMQ_ERROR_FAILED_SHM_OPEN   = 254,
    SHMEMQ_ERROR_FAILED_FTRUNCATE  = 253,
    SHMEMQ_ERROR_FAILED_MMAP       = 252,
    SHMEMQ_ERROR_FAILED_MUNMAP     = 251,
    SHMEMQ_ERROR_FAILED_CLOSE      = 250,
    SHMEMQ_ERROR_FAILED_SHM_UNLINK = 249,
};

struct ShmemqFrameHeader {
    size_t frame_size;
};

struct ShmemqFrame {
    struct ShmemqFrameHeader header;
    unsigned char data[SHMEMQ_FRAME_DATA_SIZE];
};

struct ShmemqBufferHeader {
    bool is_ready;
    size_t frames_count;
    size_t read_frame_index;
    size_t write_frame_index;
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

Shmemq shmemq_new(
    const char *name,
    bool is_consumer,
    enum ShmemqError *error_ptr
);

void shmemq_init(
    Shmemq shmemq,
    const char *name,
    bool is_consumer,
    enum ShmemqError *error_ptr
);

void shmemq_delete(Shmemq shmemq, enum ShmemqError *error_ptr);

void shmemq_finish(Shmemq shmemq, enum ShmemqError *error_ptr);

#ifdef __cplusplus
}
#endif

#endif
