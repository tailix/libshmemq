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

    for (unsigned i = 0; i < 100; ++i) {
        const ShmemqFrame frame = shmemq_push_start(producer);
        assert(frame != NULL);

        *(unsigned*)frame->data = i;

        shmemq_push_end(producer, sizeof(unsigned), &error);
        assert(error == SHMEMQ_ERROR_NONE);
    }

    assert(shmemq_push_start(producer) == NULL);

    shmemq_push_end(producer, sizeof(unsigned), &error);
    assert(error == SHMEMQ_ERROR_BUG_PUSH_END_ON_FULL_QUEUE);

    SHMEMQ_DELETE(consumer, &error);
    assert(error == SHMEMQ_ERROR_NONE);

    SHMEMQ_DELETE(producer, &error);
    assert(error == SHMEMQ_ERROR_NONE);

    return 0;
}
