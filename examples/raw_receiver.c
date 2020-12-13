#include "raw.h"

#define _POSIX_C_SOURCE 200809L

#include <shmemq.h>

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

    printf("Create queue.\n");

    enum Shmemq_Error shmemq_error;
    Shmemq shmemq = shmemq_new("/buffer1", true, &shmemq_error);

    assert(shmemq_error == SHMEMQ_ERROR_NONE);
    assert(shmemq != NULL);

    struct Queue *const queue = (void*)shmemq->buffer;

    printf("Initialize queues.\n");

    queue->read_offset = 0;

    printf("Main loop.\n");

    while (running) {
        const struct Message *const message = (struct Message*)queue->data + queue->read_offset;

        if (message->magic != BUFFER1_MAGIC) {
            printf("No messages.\n");
            sleep(1);
            continue;
        }

        if (message->size > BUFFER1_SIZE - sizeof(struct Queue)) {
            printf("Message too big.\n");
            break;
        }

        if (message->size > BUFFER1_SIZE - sizeof(struct Queue) - queue->read_offset) {
            printf("Buffer return.\n");
            queue->read_offset = 0;
            continue;
        }

        queue->read_offset += message->size;

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

    printf("Destroy queue.\n");

    shmemq_delete(shmemq);

    return 0;
}
