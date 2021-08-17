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

    ShmemqError shmemq_error;
    Shmemq shmemq = shmemq_new("/buffer1", true, &shmemq_error);

    assert(shmemq_error == SHMEMQ_ERROR_NONE);
    assert(shmemq != NULL);

    printf("Main loop.\n");

    while (running) {
        const ShmemqFrame frame = shmemq_pop_start(shmemq);

        if (frame == NULL) {
            printf("No messages.\n");
            sleep(1);
            continue;
        }

        const struct Message *const message = (struct Message*)frame->data;

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

        shmemq_pop_end(shmemq, &shmemq_error);

        if (shmemq_error != SHMEMQ_ERROR_NONE) {
            printf(
                "Error: %u (SHMEMQ_ERROR_%s).\n",
                shmemq_error,
                shmemq_error_str(shmemq_error)
            );
            break;
        }
    }

    printf("Destroy queue.\n");

    SHMEMQ_DELETE(shmemq, NULL);

    return 0;
}
