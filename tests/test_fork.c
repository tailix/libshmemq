#define _POSIX_C_SOURCE 200809L

#include <shmemq.h>

#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
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
                struct timespec tspec;
                const int clock_result = clock_gettime(CLOCK_REALTIME, &tspec);
                assert(clock_result == 0);

                tspec.tv_nsec += 1000;

                sem_timedwait(&consumer->buffer->header.read_sem, &tspec);
                continue;
            }

            ++index;

            const unsigned data = *(unsigned*)frame->data;

            printf(
                "[CONSUMER] index: %u; data: %u; frame: %p\n",
                index,
                data,
                frame
            );

            assert(data == index);

            shmemq_pop_end(consumer, &error);
            assert(error == SHMEMQ_ERROR_NONE);

            int sem_value;
            const int sem_getvalue_result =
                sem_getvalue(&consumer->buffer->header.write_sem, &sem_value);
            assert(sem_getvalue_result == 0);

            if (sem_value == 0) {
                const int sem_post_result =
                    sem_post(&consumer->buffer->header.write_sem);
                assert(sem_post_result == 0);
            }
        }
    }
    else {
        atexit(on_exit);
        signal(SIGABRT, on_signal);

        producer = shmemq_new(name, false, &error);
        assert(error == SHMEMQ_ERROR_NONE);

        for (unsigned index = 0; index < 1000;) {
            const ShmemqFrame frame = shmemq_push_start(producer);

            if (frame == NULL) {
                struct timespec tspec;
                const int clock_result = clock_gettime(CLOCK_REALTIME, &tspec);
                assert(clock_result == 0);

                tspec.tv_nsec += 1000;

                sem_timedwait(&producer->buffer->header.write_sem, &tspec);
                continue;
            }

            ++index;

            *(unsigned*)frame->data = index;

            printf(
                "[PRODUCER] index: %u; frame: %p\n",
                index,
                frame
            );

            shmemq_push_end(producer, sizeof(unsigned), &error);
            assert(error == SHMEMQ_ERROR_NONE);

            int sem_value;
            const int sem_getvalue_result =
                sem_getvalue(&producer->buffer->header.read_sem, &sem_value);
            assert(sem_getvalue_result == 0);

            if (sem_value == 0) {
                const int sem_post_result =
                    sem_post(&producer->buffer->header.read_sem);
                assert(sem_post_result == 0);
            }
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
