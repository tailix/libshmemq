#include <shmemq.h>

#include <assert.h>

int main()
{
    struct Shmemq shmemq;

    assert(shmemq_init(&shmemq, "/foobar", false) == SHMEMQ_ERROR_NONE);

    assert(shmemq_finish(&shmemq) == SHMEMQ_ERROR_NONE);

    assert(shmemq.shm_id == -1);
    assert(shmemq.buffer == NULL);

    return 0;
}
