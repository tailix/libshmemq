#include <shmemq.h>

#include <assert.h>

static const char name[] = "/foobar";

int main()
{
    ShmemqError error;

    const Shmemq consumer = shmemq_new(name, true, &error);
    assert(error == SHMEMQ_ERROR_NONE);

    const Shmemq producer = shmemq_new(name, false, &error);
    assert(error == SHMEMQ_ERROR_NONE);

    assert(shmemq_pop_start(consumer) == NULL);

    shmemq_pop_end(consumer, &error);
    assert(error == SHMEMQ_ERROR_BUG_POP_END_ON_EMPTY_QUEUE);

    shmemq_delete(consumer, &error);
    assert(error == SHMEMQ_ERROR_NONE);

    shmemq_delete(producer, &error);
    assert(error == SHMEMQ_ERROR_NONE);

    return 0;
}
