#include <shmemq.h>

#include <assert.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static const char name[] = "/foobar";

static pid_t pid = 0;
static ShmemqError error = SHMEMQ_ERROR_NONE;
static Shmemq consumer = NULL;
static Shmemq producer = NULL;

static void on_exit();
static void on_signal(int signo);

int main()
{
    pid = fork();
    assert(pid != -1);

    if (pid) {
        atexit(on_exit);
        signal(SIGABRT, on_signal);

        consumer = shmemq_new(name, true, &error);
        assert(error == SHMEMQ_ERROR_NONE);

        for (unsigned index = 0; index < 1000;) {
            const ShmemqFrame frame = shmemq_pop_start(consumer);

            if (frame == NULL) {
                sleep(1);
                continue;
            }

            ++index;

            const unsigned data = *(unsigned*)frame->data;

            assert(data == index);

            shmemq_pop_end(consumer, &error);
            assert(error == SHMEMQ_ERROR_NONE);
        }
    }
    else {
        atexit(on_exit);
        signal(SIGABRT, on_signal);

        sleep(1);

        producer = shmemq_new(name, false, &error);
        assert(error == SHMEMQ_ERROR_NONE);

        for (unsigned index = 0; index < 1000;) {
            const ShmemqFrame frame = shmemq_push_start(producer);

            if (frame == NULL) {
                sleep(1);
                continue;
            }

            ++index;

            *(unsigned*)frame->data = index;

            shmemq_push_end(producer, sizeof(unsigned), &error);
            assert(error == SHMEMQ_ERROR_NONE);
        }
    }

    return 0;
}

void on_exit()
{
    if (consumer) {
        SHMEMQ_DELETE(consumer, &error);
        assert(error == SHMEMQ_ERROR_NONE);
    }

    if (producer) {
        SHMEMQ_DELETE(producer, &error);
        assert(error == SHMEMQ_ERROR_NONE);
    }

    if (pid) {
        int wstatus;
        waitpid(pid, &wstatus, 0);
        assert(WIFEXITED(wstatus) == true);
        assert(WEXITSTATUS(wstatus) == 0);
    }
}

void on_signal(const int signo __attribute__((unused)))
{
    on_exit();
}
