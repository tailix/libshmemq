#include <shmemq.h>

#include <assert.h>
#include <string.h>

static void test(const ShmemqError shmemq_error, const char *const expected)
{
    assert(strcmp(shmemq_error_str(shmemq_error), expected) == 0);
}

int main()
{
    test(49,  "UNKNOWN");
    test(99,  "UNKNOWN");
    test(149, "UNKNOWN");

    test(SHMEMQ_ERROR_NONE,                       "NONE");
    test(SHMEMQ_ERROR_INVALID_NAME,               "INVALID_NAME");
    test(SHMEMQ_ERROR_BUG_POP_END_ON_EMPTY_QUEUE, "BUG_POP_END_ON_EMPTY_QUEUE");
    test(SHMEMQ_ERROR_BUG_PUSH_END_ON_FULL_QUEUE, "BUG_PUSH_END_ON_FULL_QUEUE");
    test(SHMEMQ_ERROR_BUG_PUSH_END_OVERFLOW,      "BUG_PUSH_END_OVERFLOW");
    test(SHMEMQ_ERROR_FAILED_MALLOC,              "FAILED_MALLOC");
    test(SHMEMQ_ERROR_FAILED_SHM_OPEN,            "FAILED_SHM_OPEN");
    test(SHMEMQ_ERROR_FAILED_FTRUNCATE,           "FAILED_FTRUNCATE");
    test(SHMEMQ_ERROR_FAILED_MMAP,                "FAILED_MMAP");
    test(SHMEMQ_ERROR_FAILED_MUNMAP,              "FAILED_MUNMAP");
    test(SHMEMQ_ERROR_FAILED_CLOSE,               "FAILED_CLOSE");
    test(SHMEMQ_ERROR_FAILED_SHM_UNLINK,          "FAILED_SHM_UNLINK");
    test(SHMEMQ_ERROR_FAILED_SEM_INIT,            "FAILED_SEM_INIT");

    return 0;
}
