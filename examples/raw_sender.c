#include "raw.h"

#define _GNU_SOURCE

#include <assert.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static const char nullstr[] = "Hello, World!";

int main()
{
    printf("Open shared memory objects.\n");

    int indicator_shm_id = shm_open(
        "/indicator",
        O_RDONLY,
        S_IRUSR | S_IWUSR
    );

    int buffer1_shm_id = shm_open(
        "/buffer1",
        O_RDWR,
        S_IRUSR | S_IWUSR
    );

    assert(indicator_shm_id != -1);
    assert(buffer1_shm_id   != -1);

    printf("Create memory mappings.\n");

    struct Indicator *const indicator = mmap(
        NULL,
        sizeof(*indicator),
        PROT_READ,
        MAP_SHARED,
        indicator_shm_id,
        0
    );

    void *const buffer1 = mmap(
        NULL,
        BUFFER1_SIZE,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        buffer1_shm_id,
        0
    );

    assert(indicator != MAP_FAILED);
    assert(buffer1   != MAP_FAILED);

    printf("Initialize queues.\n");

    size_t buffer1_offset = indicator->buffer1_offset;

    for (;;) {
        const struct Message *const message = buffer1 + buffer1_offset;

        if (message->magic != BUFFER1_MAGIC) break;

        if (message->size > BUFFER1_SIZE) {
            printf("Message too big.\n");
            goto finalize;
        }

        if (message->size > BUFFER1_SIZE - buffer1_offset) {
            printf("Buffer return.\n");
            buffer1_offset = 0;
            continue;
        }

        buffer1_offset += message->size;
    }

    printf(
        "REPL commands:\n"
        "  x - exit\n"
        "  f - send finish message\n"
        "  1 - send 1 byte\n"
        "  0 - send null-terminated string\n"
    );

    printf("REPL.\n");

    for (;;) {
        const char chr = getchar();

        if (chr == '\n') continue;

        if (chr == 'x') {
            printf("Command: exit.\n");
            break;
        }

        struct Message *message = buffer1 + buffer1_offset;

        switch (chr) {
        case 'f':
            {
                const size_t size = sizeof(*message);
                buffer1_offset += size;
                message->type = FINISH;
                message->size = size;
                message->magic = BUFFER1_MAGIC;
            }
            break;
        case '1':
            {
                const size_t size = sizeof(*message) + sizeof(unsigned char);
                buffer1_offset += size;
                *(unsigned char*)message->data = 123;
                message->type = ONEBYTE;
                message->size = size;
                message->magic = BUFFER1_MAGIC;
            }
            break;
        case '0':
            {
                const size_t size = sizeof(*message) + strlen(nullstr) + 1;
                buffer1_offset += size;
                strcpy((char*)message->data, nullstr);
                message->type = NULLSTR;
                message->size = size;
                message->magic = BUFFER1_MAGIC;
            }
            break;
        default:
            printf("Unknown command: '%c'\n", chr);
        }
    }

finalize:
    printf("Destroy memory mappings.\n");

    assert(munmap(indicator, sizeof(*indicator)) == 0);
    assert(munmap(buffer1,   BUFFER1_SIZE)       == 0);

    printf("Close shared memory objects.\n");

    assert(close(indicator_shm_id) == 0);
    assert(close(buffer1_shm_id)   == 0);

    return 0;
}
