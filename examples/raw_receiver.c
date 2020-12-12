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

static bool running = true;

static void signal_handler(const int signo)
{
    if (signo == SIGINT) {
        running = false;
    }
}

int main()
{
    signal(SIGINT, signal_handler);

    printf("Open shared memory objects.\n");

    int indicator_shm_id = shm_open(
        "/indicator",
        O_CREAT | O_EXCL | O_RDWR,
        S_IRUSR | S_IWUSR
    );

    int buffer1_shm_id = shm_open(
        "/buffer1",
        O_CREAT | O_EXCL | O_RDWR,
        S_IRUSR | S_IWUSR
    );

    assert(indicator_shm_id != -1);
    assert(buffer1_shm_id   != -1);

    printf("Truncate shared memory objects.\n");

    assert(ftruncate(indicator_shm_id, sizeof(struct Indicator)) == 0);
    assert(ftruncate(buffer1_shm_id,   BUFFER1_SIZE)             == 0);

    printf("Create memory mappings.\n");

    struct Indicator *const indicator = mmap(
        NULL,
        sizeof(*indicator),
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        indicator_shm_id,
        0
    );

    void *const buffer1 = mmap(
        NULL,
        BUFFER1_SIZE,
        PROT_READ,
        MAP_SHARED,
        buffer1_shm_id,
        0
    );

    assert(indicator != MAP_FAILED);
    assert(buffer1   != MAP_FAILED);

    printf("Initialize queues.\n");

    indicator->buffer1_offset  = 0;

    printf("Main loop.\n");

    while (running) {
        const struct Message *const message = buffer1 + indicator->buffer1_offset;

        if (message->magic != BUFFER1_MAGIC) {
            printf("No messages.\n");
            sleep(1);
            continue;
        }

        if (message->size > BUFFER1_SIZE) {
            printf("Message too big.\n");
            break;
        }

        if (message->size > BUFFER1_SIZE - indicator->buffer1_offset) {
            printf("Buffer return.\n");
            indicator->buffer1_offset = 0;
            continue;
        }

        indicator->buffer1_offset += message->size;

        switch (message->type) {
        case FINISH:
            printf("Message: finish.\n");
            running = false;
            break;
        case ONEBYTE:
            printf("Message: 1 byte (%u)\n", *(unsigned char*)message->data);
            break;
        case NULLSTR:
            printf("Message: null-terminated string (%s)\n", message->data);
            break;
        default:
            printf("Invalid message.\n");
            running = false;
        }
    }

    printf("Destroy memory mappings.\n");

    assert(munmap(indicator, sizeof(*indicator)) == 0);
    assert(munmap(buffer1,   BUFFER1_SIZE)       == 0);

    printf("Unlink shared memory objects.\n");

    assert(shm_unlink("/indicator") == 0);
    assert(shm_unlink("/buffer1")   == 0);

    return 0;
}
