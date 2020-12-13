#ifndef SHMEMQ_INCLUDED
#define SHMEMQ_INCLUDED 1

#include <stdbool.h>
#include <stddef.h>

#define SHMEMQ_NAME_SIZE_MAX ((size_t)255)
#define SHMEMQ_NAME_SLEN_MAX (SHMEMQ_NAME_SIZE_MAX - 1)

// TODO: Resize buffer dynamically.
// #define SHMEMQ_BUFFER_SIZE_MIN (sizeof(struct Shmemq_BufferHeader))
#define SHMEMQ_BUFFER_SIZE_MIN (sizeof(struct Shmemq_BufferHeader) + 800)

#define SHMEMQ_FRAME_SIZE ((size_t)8)

#define SHMEMQ_FRAME_DATA_SIZE \
    (SHMEMQ_FRAME_SIZE - sizeof(struct Shmemq_FrameHeader))

#ifdef __cplusplus
extern "C" {
#endif

enum Shmemq_Error {
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

struct Shmemq_FrameHeader {
    size_t frame_size;
};

struct Shmemq_Frame {
    struct Shmemq_FrameHeader header;
    unsigned char data[SHMEMQ_FRAME_DATA_SIZE];
};

struct Shmemq_BufferHeader {
    bool is_ready;
    size_t frames_count;
    size_t read_frame_index;
    size_t write_frame_index;
};

struct Shmemq_Buffer {
    struct Shmemq_BufferHeader header;
    struct Shmemq_Frame frames[];
};

typedef struct Shmemq {
    char name[SHMEMQ_NAME_SIZE_MAX];
    bool is_consumer;
    int shm_id;
    struct Shmemq_Buffer *buffer;
} *Shmemq;

Shmemq shmemq_new(
    const char *name,
    bool is_consumer,
    enum Shmemq_Error *error_ptr
);

void shmemq_init(
    Shmemq shmemq,
    const char *name,
    bool is_consumer,
    enum Shmemq_Error *error_ptr
);

void shmemq_delete(Shmemq shmemq, enum Shmemq_Error *error_ptr);

void shmemq_finish(Shmemq shmemq, enum Shmemq_Error *error_ptr);

#ifdef __cplusplus
}
#endif

#endif
