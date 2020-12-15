#include <shmemq.h>

#include <assert.h>
#include <signal.h>
#include <stdlib.h>

static const char name[] = "/foobar";

static ShmemqError error = SHMEMQ_ERROR_NONE;
static Shmemq consumer = NULL;
static Shmemq producer = NULL;

static void on_exit();
static void on_signal(int signo);

int main()
{
    atexit(on_exit);
    signal(SIGABRT, on_signal);

    consumer = shmemq_new(name, true, &error);
    assert(error == SHMEMQ_ERROR_NONE);

    producer = shmemq_new(name, false, &error);
    assert(error == SHMEMQ_ERROR_NONE);

    for (unsigned i = 0; i < 100; ++i) {
        const ShmemqFrame frame = shmemq_push_start(producer);
        assert(frame != NULL);

        *(unsigned*)frame->data = i;

        shmemq_push_end(producer, sizeof(unsigned), &error);
        assert(error == SHMEMQ_ERROR_NONE);
    }

    for (unsigned i = 0; i < 10; ++i) {
        const ShmemqFrame frame = shmemq_pop_start(consumer);
        assert(frame != NULL);

        assert(*(unsigned*)frame->data == i);

        shmemq_pop_end(consumer, &error);
        assert(error == SHMEMQ_ERROR_NONE);
    }

    for (unsigned i = 0; i < 8; ++i) {
        const ShmemqFrame frame = shmemq_push_start(producer);
        assert(frame != NULL);

        *(unsigned*)frame->data = i;

        shmemq_push_end(producer, sizeof(unsigned), &error);
        assert(error == SHMEMQ_ERROR_NONE);
    }

    const ShmemqFrame frame = shmemq_push_start(producer);
    assert(frame != NULL);

    shmemq_push_end(producer, 9, &error);
    assert(error == SHMEMQ_ERROR_BUG_PUSH_END_OVERFLOW);

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
}

void on_signal(const int signo __attribute__((unused)))
{
    on_exit();
}
