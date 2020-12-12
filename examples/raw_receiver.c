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

    int buffer1_shm_id = shm_open(
        "/buffer1",
        O_CREAT | O_EXCL | O_RDWR,
        S_IRUSR | S_IWUSR
    );

    assert(buffer1_shm_id != -1);

    printf("Truncate shared memory objects.\n");

    assert(ftruncate(buffer1_shm_id, BUFFER1_SIZE) == 0);

    printf("Create memory mappings.\n");

    struct Queue *const queue = mmap(
        NULL,
        BUFFER1_SIZE,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        buffer1_shm_id,
        0
    );

    assert(queue != MAP_FAILED);

    printf("Initialize queues.\n");

    queue->offset = 0;

    printf("Main loop.\n");

    while (running) {
        const struct Message *const message = (struct Message*)queue->data + queue->offset;

        if (message->magic != BUFFER1_MAGIC) {
            printf("No messages.\n");
            sleep(1);
            continue;
        }

        if (message->size > BUFFER1_SIZE - sizeof(struct Queue)) {
            printf("Message too big.\n");
            break;
        }

        if (message->size > BUFFER1_SIZE - sizeof(struct Queue) - queue->offset) {
            printf("Buffer return.\n");
            queue->offset = 0;
            continue;
        }

        queue->offset += message->size;

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

    assert(munmap(queue, BUFFER1_SIZE) == 0);

    printf("Unlink shared memory objects.\n");

    assert(shm_unlink("/buffer1") == 0);

    return 0;
}
