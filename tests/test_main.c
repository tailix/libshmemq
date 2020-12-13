#include <shmemq.h>

#include <assert.h>
#include <string.h>
#include <sys/mman.h>

static const char name[] = "/foobar";

int main()
{
    struct Shmemq consumer_shmemq;
    memset(&consumer_shmemq, 0, sizeof(consumer_shmemq));

    assert(shmemq_init(&consumer_shmemq, name, true) == SHMEMQ_ERROR_NONE);

    assert(strcmp(consumer_shmemq.name, name) == 0);
    assert(consumer_shmemq.is_consumer == true);
    assert(consumer_shmemq.shm_id != -1);
    assert(consumer_shmemq.buffer != NULL);
    assert(consumer_shmemq.buffer->header.is_ready == false);
    assert(consumer_shmemq.buffer->header.frames_count == 0);
    assert(consumer_shmemq.buffer->header.read_frame_index == 0);
    assert(consumer_shmemq.buffer->header.write_frame_index == 0);

    enum Shmemq_Error error;
    struct Shmemq *producer_shmemq = shmemq_new(name, false, &error);

    assert(producer_shmemq != NULL);
    assert(error == SHMEMQ_ERROR_NONE);

    assert(strcmp(producer_shmemq->name, name) == 0);
    assert(producer_shmemq->is_consumer == false);
    assert(producer_shmemq->shm_id != -1);
    assert(producer_shmemq->buffer != NULL);
    assert(producer_shmemq->buffer->header.is_ready == false);
    assert(producer_shmemq->buffer->header.frames_count == 0);
    assert(producer_shmemq->buffer->header.read_frame_index == 0);
    assert(producer_shmemq->buffer->header.write_frame_index == 0);

    assert(consumer_shmemq.buffer != producer_shmemq->buffer);

    consumer_shmemq.buffer->header.is_ready = true;
    assert(producer_shmemq->buffer->header.is_ready == true);

    assert(shmemq_finish(&consumer_shmemq) == SHMEMQ_ERROR_NONE);
    assert(shmemq_delete(producer_shmemq) == SHMEMQ_ERROR_NONE);

    return 0;
}
