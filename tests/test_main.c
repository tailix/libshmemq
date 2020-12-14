#include <shmemq.h>

#include <assert.h>
#include <string.h>
#include <sys/mman.h>

static const char name[] = "/foobar";
static const char hello_world_str[] = "Hello, World!";

int main()
{
    ShmemqError error;

    struct Shmemq consumer_shmemq;
    memset(&consumer_shmemq, 0, sizeof(consumer_shmemq));

    shmemq_init(&consumer_shmemq, name, true, &error);
    assert(error == SHMEMQ_ERROR_NONE);

    assert(strcmp(consumer_shmemq.name, name) == 0);
    assert(consumer_shmemq.is_consumer == true);
    assert(consumer_shmemq.shm_id != -1);
    assert(consumer_shmemq.buffer != NULL);
    assert(consumer_shmemq.buffer->header.is_ready == false);
    // TODO: Resize buffer dynamically.
    // assert(consumer_shmemq.buffer->header.frames_count == 0);
    assert(consumer_shmemq.buffer->header.frames_count == 100);
    assert(consumer_shmemq.buffer->header.read_frame_index == 0);
    assert(consumer_shmemq.buffer->header.write_frame_index == 0);

    const Shmemq producer_shmemq = shmemq_new(name, false, &error);

    assert(producer_shmemq != NULL);
    assert(error == SHMEMQ_ERROR_NONE);

    assert(strcmp(producer_shmemq->name, name) == 0);
    assert(producer_shmemq->is_consumer == false);
    assert(producer_shmemq->shm_id != -1);
    assert(producer_shmemq->buffer != NULL);
    assert(producer_shmemq->buffer->header.is_ready == false);
    // TODO: Resize buffer dynamically.
    // assert(producer_shmemq->buffer->header.frames_count == 0);
    assert(producer_shmemq->buffer->header.frames_count == 100);
    assert(producer_shmemq->buffer->header.read_frame_index == 0);
    assert(producer_shmemq->buffer->header.write_frame_index == 0);

    assert(consumer_shmemq.buffer != producer_shmemq->buffer);

    consumer_shmemq.buffer->header.is_ready = true;
    assert(producer_shmemq->buffer->header.is_ready == true);

    ShmemqFrame frame = shmemq_push_start(producer_shmemq);

    assert(frame == &producer_shmemq->buffer->frames[0]);

    *(unsigned int*)frame->data = 123;
    shmemq_push_end(producer_shmemq, sizeof(unsigned int));

    assert(consumer_shmemq.buffer->header.read_frame_index == 0);
    assert(consumer_shmemq.buffer->header.write_frame_index == 1);
    assert(consumer_shmemq.buffer->frames[0].header.message_frames_count == 1);
    assert(consumer_shmemq.buffer->frames[0].data[0] == 123);
    assert(consumer_shmemq.buffer->frames[0].data[1] == 0);
    assert(consumer_shmemq.buffer->frames[0].data[2] == 0);
    assert(consumer_shmemq.buffer->frames[0].data[3] == 0);

    frame = shmemq_push_start(producer_shmemq);

    assert(frame == &producer_shmemq->buffer->frames[1]);

    *(unsigned int*)frame->data = 456;
    shmemq_push_end(producer_shmemq, sizeof(unsigned int));

    assert(consumer_shmemq.buffer->header.read_frame_index == 0);
    assert(consumer_shmemq.buffer->header.write_frame_index == 2);
    assert(consumer_shmemq.buffer->frames[0].header.message_frames_count == 1);
    assert(consumer_shmemq.buffer->frames[0].data[0] == 123);
    assert(consumer_shmemq.buffer->frames[0].data[1] == 0);
    assert(consumer_shmemq.buffer->frames[0].data[2] == 0);
    assert(consumer_shmemq.buffer->frames[0].data[3] == 0);
    assert(consumer_shmemq.buffer->frames[1].header.message_frames_count == 1);
    assert(consumer_shmemq.buffer->frames[1].data[0] == 200);
    assert(consumer_shmemq.buffer->frames[1].data[1] == 1);
    assert(consumer_shmemq.buffer->frames[1].data[2] == 0);
    assert(consumer_shmemq.buffer->frames[1].data[3] == 0);

    frame = shmemq_push_start(producer_shmemq);

    assert(frame == &producer_shmemq->buffer->frames[2]);

    strcpy((char*)frame->data, hello_world_str);
    shmemq_push_end(producer_shmemq, strlen(hello_world_str) + 1);

    assert(consumer_shmemq.buffer->header.read_frame_index == 0);
    assert(consumer_shmemq.buffer->header.write_frame_index == 4);
    assert(consumer_shmemq.buffer->frames[0].header.message_frames_count == 1);
    assert(consumer_shmemq.buffer->frames[0].data[0] == 123);
    assert(consumer_shmemq.buffer->frames[0].data[1] == 0);
    assert(consumer_shmemq.buffer->frames[0].data[2] == 0);
    assert(consumer_shmemq.buffer->frames[0].data[3] == 0);
    assert(consumer_shmemq.buffer->frames[1].header.message_frames_count == 1);
    assert(consumer_shmemq.buffer->frames[1].data[0] == 200);
    assert(consumer_shmemq.buffer->frames[1].data[1] == 1);
    assert(consumer_shmemq.buffer->frames[1].data[2] == 0);
    assert(consumer_shmemq.buffer->frames[1].data[3] == 0);
    assert(consumer_shmemq.buffer->frames[2].header.message_frames_count == 2);
    assert(
        strcmp(
            (char*)consumer_shmemq.buffer->frames[2].data,
            hello_world_str
        ) == 0
    );

    shmemq_finish(&consumer_shmemq, &error);
    assert(error == SHMEMQ_ERROR_NONE);

    shmemq_delete(producer_shmemq, &error);
    assert(error == SHMEMQ_ERROR_NONE);

    return 0;
}
