#ifndef _RAW_H
#define _RAW_H 1

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

enum MessageType {
    FINISH,
    ONEBYTE,
    NULLSTR,
};

struct Message {
    enum MessageType type;
    unsigned char data[];
};

#ifdef __cplusplus
}
#endif

#endif
