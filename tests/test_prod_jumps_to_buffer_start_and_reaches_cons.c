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

    for (unsigned i = 0; i < 10; ++i) {
        const ShmemqFrame frame = shmemq_push_start(producer);
        assert(frame != NULL);

        *(unsigned*)frame->data = i;

        shmemq_push_end(producer, sizeof(unsigned), &error);
        assert(error == SHMEMQ_ERROR_NONE);
    }

    assert(shmemq_push_start(producer) == NULL);

    shmemq_push_end(producer, sizeof(unsigned), &error);
    assert(error == SHMEMQ_ERROR_BUG_PUSH_END_ON_FULL_QUEUE);

    shmemq_delete(consumer, &error);
    assert(error == SHMEMQ_ERROR_NONE);

    shmemq_delete(producer, &error);
    assert(error == SHMEMQ_ERROR_NONE);

    return 0;
}
