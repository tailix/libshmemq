#include <shmemq.h>

#include <assert.h>

static const char name[] = "/foobar";

static Shmemq consumer = NULL;
static Shmemq producer = NULL;

int main()
{
    ShmemqError error;

    consumer = shmemq_new(name, true, &error);
    assert(error == SHMEMQ_ERROR_NONE);

    producer = shmemq_new(name, false, &error);
    assert(error == SHMEMQ_ERROR_NONE);

    assert(shmemq_pop_start(consumer) == NULL);

    shmemq_pop_end(consumer, &error);
    assert(error == SHMEMQ_ERROR_BUG_POP_END_ON_EMPTY_QUEUE);

    SHMEMQ_DELETE(consumer, &error);
    assert(error == SHMEMQ_ERROR_NONE);

    SHMEMQ_DELETE(producer, &error);
    assert(error == SHMEMQ_ERROR_NONE);

    return 0;
}
