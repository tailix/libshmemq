#ifndef _RAW_H
#define _RAW_H 1

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

static const size_t BUFFER1_SIZE  = 200;
static const long   BUFFER1_MAGIC = 0xCAFEBABE;

struct Indicator {
    size_t buffer1_offset;
};

enum MessageType {
    FINISH,
    ONEBYTE,
    NULLSTR,
};

struct Message {
    long magic;
    size_t size;
    enum MessageType type;

    unsigned char data[];
};

#ifdef __cplusplus
}
#endif

#endif
