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

    assert(shmemq_pop_start(consumer) == NULL);

    shmemq_pop_end(consumer, &error);
    assert(error == SHMEMQ_ERROR_BUG_POP_END_ON_EMPTY_QUEUE);

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
