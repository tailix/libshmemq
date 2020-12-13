#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <shmemq.h>
#include <shmemq/dummy.h>

int main()
{
    shmemq_dummy();

    return 0;
}
