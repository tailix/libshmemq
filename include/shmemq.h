#ifndef SHMEMQ_INCLUDED
#define SHMEMQ_INCLUDED 1

#include <stdbool.h>
#include <stddef.h>

#define SHMEMQ_NAME_SIZE_MAX ((size_t)255)

#define SHMEMQ_FRAME_SIZE ((size_t)8)

#define SHMEMQ_FRAME_DATA_SIZE \
  (SHMEMQ_FRAME_SIZE - sizeof(struct Shmemq_FrameHeader))

#ifdef __cplusplus
extern "C" {
#endif

struct Shmemq_FrameHeader {
    size_t frame_size;
};

struct Shmemq_Frame {
    struct Shmemq_FrameHeader header;
    unsigned char data[SHMEMQ_FRAME_DATA_SIZE];
};

struct Shmemq_BufferHeader {
    size_t frames_count;
    size_t read_frame_index;
    size_t write_frame_index;
};

struct Shmemq_Buffer {
    struct Shmemq_BufferHeader header;
    struct Shmemq_Frame frames[];
};

struct Shmemq {
    char name[SHMEMQ_NAME_SIZE_MAX];
    bool is_consumer;
    struct Shmemq_Buffer *buffer;
};

#ifdef __cplusplus
}
#endif

#endif
