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

    ShmemqError shmemq_error;
    Shmemq shmemq = shmemq_new("/buffer1", false, &shmemq_error);

    assert(shmemq_error == SHMEMQ_ERROR_NONE);
    assert(shmemq != NULL);

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

        const ShmemqFrame frame = shmemq_push_start(shmemq);

        struct Message *const message = (struct Message*)frame->data;

        switch (chr) {
        case 'f':
            {
                const size_t size = sizeof(*message);
                message->type = FINISH;
                shmemq_push_end(shmemq, size, &shmemq_error);

                if (shmemq_error != SHMEMQ_ERROR_NONE) goto finalize;
            }
            break;
        case '1':
            {
                const size_t size = sizeof(*message) + sizeof(unsigned char);
                *(unsigned char*)message->data = 123;
                message->type = ONEBYTE;
                shmemq_push_end(shmemq, size, &shmemq_error);

                if (shmemq_error != SHMEMQ_ERROR_NONE) goto finalize;
            }
            break;
        case '0':
            {
                const size_t size = sizeof(*message) + strlen(nullstr) + 1;
                strcpy((char*)message->data, nullstr);
                message->type = NULLSTR;
                shmemq_push_end(shmemq, size, &shmemq_error);

                if (shmemq_error != SHMEMQ_ERROR_NONE) goto finalize;
            }
            break;
        default:
            printf("Unknown command: '%c'\n", chr);
        }
    }

finalize:
    if (shmemq_error != SHMEMQ_ERROR_NONE) printf("Error: %u.\n", shmemq_error);

    printf("Destroy queue.\n");

    SHMEMQ_DELETE(shmemq, NULL);

    return 0;
}
