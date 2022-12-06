libshmemq
=========

[![Build Status](https://travis-ci.org/kotovalexarian/libshmemq.svg?branch=master)](https://travis-ci.org/kotovalexarian/libshmemq)

Library for message queue in shared memory.



Table of contents
-----------------

* [Overview](#libshmemq)
* [Table of contents](#table-of-contents)
* [Links](#links)
  * [Similar implementations](#similar-implementations)
  * [Implementation theory](#implementation-theory)
  * [Shared memory and other IPC theory](#shared-memory-and-other-ipc-theory)
  * [Cost theory](#cost-theory)



Links
-----

### Similar implementations

* [Boost Lock-free](https://www.boost.org/doc/libs/1_77_0/doc/html/lockfree.html)
* [rmind/ringbuf](https://github.com/rmind/ringbuf)
* [goldshtn/shmemq-blog](https://github.com/goldshtn/shmemq-blog)
* [MengRao/SPSC_Queue](https://github.com/MengRao/SPSC_Queue)
* [ezhang887/shmemq](https://github.com/ezhang887/shmemq)
* [MengRao/tcpshm](https://github.com/MengRao/tcpshm)

### Implementation theory

* [Building a shared memory IPC implementation – Part I](https://coherent-labs.com/posts/building-a-shared-memory-ipc-implementation-part-i/)
* [Single-Producer/Single-Consumer Queue](https://software.intel.com/content/www/us/en/develop/articles/single-producer-single-consumer-queue.html)
* [A lock-free, cache-efficient shared ring buffer for multi-core architectures](https://www.researchgate.net/publication/221046035_A_lock-free_cache-efficient_shared_ring_buffer_for_multi-core_architectures)

### Shared memory and other IPC theory

* [What are primitives for the fastest user-space IPC?](https://forum.osdev.org/viewtopic.php?f=1&t=38693)
* [Разделяемая память. Семафоры.](https://youtu.be/g_qco-EJqDM)

### Cost theory

* [Which takes longer time? Switching between the user & kernel modes or switching between two processes?](https://stackoverflow.com/a/14205346)
* [What happens the most, context switches or mode switches?](https://unix.stackexchange.com/a/15537)
