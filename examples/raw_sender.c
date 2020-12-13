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

static const char nullstr[] = "Hello, World!";

int main()
{
    printf("Create queue.\n");

    enum Shmemq_Error shmemq_error;
    struct Shmemq *shmemq = shmemq_new("/buffer1", true, &shmemq_error);

    assert(shmemq_error == SHMEMQ_ERROR_NONE);
    assert(shmemq != NULL);

    struct Queue *const queue = (void*)shmemq->buffer;

    printf("Initialize queues.\n");

    size_t buffer1_offset = queue->read_offset;

    for (;;) {
        const struct Message *const message = (struct Message*)queue->data + buffer1_offset;

        if (message->magic != BUFFER1_MAGIC) break;

        if (message->size > BUFFER1_SIZE - sizeof(struct Queue)) {
            printf("Message too big.\n");
            goto finalize;
        }

        if (message->size > BUFFER1_SIZE - sizeof(struct Queue) - buffer1_offset) {
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

        struct Message *message = (struct Message*)queue->data + buffer1_offset;

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
    printf("Destroy queue.\n");

    shmemq_delete(shmemq);

    return 0;
}
