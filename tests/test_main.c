#include <shmemq.h>

#include <assert.h>
#include <string.h>
#include <sys/mman.h>

static const char name[] = "/foobar";

int main()
{
    struct Shmemq consumer_shmemq;
    struct Shmemq producer_shmemq;

    memset(&consumer_shmemq, 0, sizeof(consumer_shmemq));
    memset(&producer_shmemq, 0, sizeof(producer_shmemq));

    assert(shmemq_init(&consumer_shmemq, name, true) == SHMEMQ_ERROR_NONE);

    assert(strcmp(consumer_shmemq.name, name) == 0);
    assert(consumer_shmemq.is_consumer == true);
    assert(consumer_shmemq.shm_id != -1);
    assert(consumer_shmemq.buffer != NULL);
    assert(consumer_shmemq.buffer->header.frames_count == 0);
    assert(consumer_shmemq.buffer->header.read_frame_index == 0);
    assert(consumer_shmemq.buffer->header.write_frame_index == 0);

    assert(shmemq_init(&producer_shmemq, name, false) == SHMEMQ_ERROR_NONE);

    assert(strcmp(producer_shmemq.name, name) == 0);
    assert(producer_shmemq.is_consumer == false);
    assert(producer_shmemq.shm_id != -1);
    assert(producer_shmemq.buffer != NULL);
    assert(producer_shmemq.buffer->header.frames_count == 0);
    assert(producer_shmemq.buffer->header.read_frame_index == 0);
    assert(producer_shmemq.buffer->header.write_frame_index == 0);

    assert(shmemq_finish(&consumer_shmemq) == SHMEMQ_ERROR_NONE);
    assert(shmemq_finish(&producer_shmemq) == SHMEMQ_ERROR_NONE);

    return 0;
}
