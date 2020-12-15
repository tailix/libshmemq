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
  * [Cost theory](#cost-theory)



Links
-----

### Similar implementations

* [goldshtn/shmemq-blog](https://github.com/goldshtn/shmemq-blog)
* [MengRao/SPSC_Queue](https://github.com/MengRao/SPSC_Queue)
* [ezhang887/shmemq](https://github.com/ezhang887/shmemq)

### Implementation theory

* [Building a shared memory IPC implementation – Part I](https://coherent-labs.com/posts/building-a-shared-memory-ipc-implementation-part-i/)
* [Single-Producer/Single-Consumer Queue](https://software.intel.com/content/www/us/en/develop/articles/single-producer-single-consumer-queue.html)

### Cost theory

* [Which takes longer time? Switching between the user & kernel modes or switching between two processes?](https://stackoverflow.com/a/14205346)
* [What happens the most, context switches or mode switches?](https://unix.stackexchange.com/a/15537)
