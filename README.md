libshmemq
=========

[![Build Status](https://travis-ci.org/kotovalexarian/libshmemq.svg?branch=master)](https://travis-ci.org/kotovalexarian/libshmemq)

Library for message queue in shared memory.



Table of contents
-----------------

* [Overview](#libshmemq)
* [Table of contents](#table-of-contents)
* [Concept](#concept)
* [External links](#external-links)



Concept
-------

Start:

```
  |
 _V_ ___ ___ ___ ___ ___ ___ ___ ___ ___
|   |   |   |   |   |   |   |   |   |   |
|___|___|___|___|___|___|___|___|___|___|
  |
  V
```

After some pushes:

```
                      |
 ___ ___ ___ ___ ___ _V_ ___ ___ ___ ___
|1  |2  |3  |4  |5  |   |   |   |   |   |
|___|___|___|___|___|___|___|___|___|___|
  |
  V
```

After some pops:

```
                      |
 ___ ___ ___ ___ ___ _V_ ___ ___ ___ ___
|   |   |3  |4  |5  |   |   |   |   |   |
|___|___|___|___|___|___|___|___|___|___|
          |
          V
```

Producer pushes fast:

```
                                      |
 ___ ___ ___ ___ ___ ___ ___ ___ ___ _V_
|   |   |3  |4  |5  |6  |7  |8  |9  |   |
|___|___|___|___|___|___|___|___|___|___|
          |
          V
```

Producer returns to the beginning:

```
  |
 _V_ ___ ___ ___ ___ ___ ___ ___ ___ ___
|   |   |3  |4  |5  |6  |7  |8  |9  |10 |
|___|___|___|___|___|___|___|___|___|___|
          |
          V
```

Producer can't push more so he waits:

```
          |
 ___ ___ _V_ ___ ___ ___ ___ ___ ___ ___
|11 |12 |3  |4  |5  |6  |7  |8  |9  |10 |
|___|___|___|___|___|___|___|___|___|___|
          |
          V
```

Consumer starts poping fast:

```
          |
 ___ ___ _V_ ___ ___ ___ ___ ___ ___ ___
|11 |12 |   |   |   |   |   |   |   |10 |
|___|___|___|___|___|___|___|___|___|___|
                                      |
                                      V
```

Consumer returns to the beginning:

```
          |
 ___ ___ _V_ ___ ___ ___ ___ ___ ___ ___
|11 |12 |   |   |   |   |   |   |   |   |
|___|___|___|___|___|___|___|___|___|___|
  |
  V
```

Consumer can't pop more so he waits:

```
          |
 ___ ___ _V_ ___ ___ ___ ___ ___ ___ ___
|   |   |   |   |   |   |   |   |   |   |
|___|___|___|___|___|___|___|___|___|___|
          |
          V
```

Producer pushes more:

```
                  |
 ___ ___ ___ ___ _V_ ___ ___ ___ ___ ___
|   |   |13 |14 |   |   |   |   |   |   |
|___|___|___|___|___|___|___|___|___|___|
          |
          V
```

Consumer pops more:

```
                  |
 ___ ___ ___ ___ _V_ ___ ___ ___ ___ ___
|   |   |   |14 |   |   |   |   |   |   |
|___|___|___|___|___|___|___|___|___|___|
              |
              V
```

Producer pushes one short and one long message:

```
                                  |
 ___ ___ ___ ___ ___ ___ ___ ___ _V_ ___
|   |   |   |14 |15         |16 |   |   |
|___|___|___|___|___ ___ ___|___|___|___|
              |
              V
```

Consumer pops one short and one long message:

```
                                  |
 ___ ___ ___ ___ ___ ___ ___ ___ _V_ ___
|   |   |   |   |   |   |   |16 |   |   |
|___|___|___|___|___|___|___|___|___|___|
                              |
                              V
```

Producer is in trouble now. He wants to send a long message, but he is
positioned at the second half of the buffer. The situation is complicated by the
fact that the size of the message may not be known in advance (stream-like
message queuing). To reduce the likelihood of it stalling due to lack of space,
it chooses whether to write to the current position or to the beginning of the
buffer, depending on where there is more free space at the moment. He compares
the following values:

* `Size of the buffer - Current position of the producer`
* `Current position of the consumer`

In our example, the second one is greater (remember that indices start with
zero):

```
                           10 - 8 = 2
                                   _A_
                                  /   \
                                  |   |
 ___ ___ ___ ___ ___ ___ ___ ___ _V_ _|_
|   |   |   |   |   |   |   |16 |   |   |
|___|___|___|___|___|___|___|___|___|___|
  |                       |   |
  |                       |   V
  \____________ __________/
               V
       7 - 0 = 7
```

So producer makes himself to point to the beginning of the buffer.

**NOTICE**: It marks the frame it pointed to earlier in a special way (indicated
with `*`).  Remember this fact, we'll use it in future.

```
  |
 _V_ ___ ___ ___ ___ ___ ___ ___ ___ ___
|   |   |   |   |   |   |   |16 |*  |   |
|___|___|___|___|___|___|___|___|___|___|
                              |
                              V
```

Consumer pops one message:

```
  |
 _V_ ___ ___ ___ ___ ___ ___ ___ ___ ___
|   |   |   |   |   |   |   |   |*  |   |
|___|___|___|___|___|___|___|___|___|___|
                                  |
                                  V
```

Producer pushes one message:

```
      |
 ___ _V_ ___ ___ ___ ___ ___ ___ ___ ___
|17 |   |   |   |   |   |   |   |*  |   |
|___|___|___|___|___|___|___|___|___|___|
                                  |
                                  V
```

Consumer is also in trouble now: how can he understand if he should try to read
next frame or go the beginning of the buffer? We can differentiate those
situations because we indicate consumed messages with no number, but in reality
corresponding frames contain grabage.

But why not to fill those frames with special value? First frame of a message
contains the number of frames in the message. It can't be zero. Can we set it to
zero for consumed frames?

Let's start with a caution. Message can consist of multiple frames, and only the
first one will have such attribute. Can we guarantee that consumer is pointed to
the frame which was the first frame of some consumed message? No, we can't.

However we can overwrite garbage when producer decides to start from the
beginning of the buffer. Do you remember the **NOTICE**? We only consider this
when position of producer is less than position of consumer. In this case if we
see frame with size greater than zero, it unequivocally signs that there is a
message, not a garbage.

Consumer makes himself to point to the beginning of the buffer:

```
      |
 ___ _V_ ___ ___ ___ ___ ___ ___ ___ ___
|17 |   |   |   |   |   |   |   |   |   |
|___|___|___|___|___|___|___|___|___|___|
  |
  V
```

Consumer pops one message:

```
      |
 ___ _V_ ___ ___ ___ ___ ___ ___ ___ ___
|   |   |   |   |   |   |   |   |   |   |
|___|___|___|___|___|___|___|___|___|___|
      |
      V
```



External links
--------------

* [goldshtn/shmemq-blog](https://github.com/goldshtn/shmemq-blog)
* [MengRao/SPSC_Queue](https://github.com/MengRao/SPSC_Queue)
* [ezhang887/shmemq](https://github.com/ezhang887/shmemq)
* [Building a shared memory IPC implementation – Part I](https://coherent-labs.com/posts/building-a-shared-memory-ipc-implementation-part-i/)
* [Single-Producer/Single-Consumer Queue](https://software.intel.com/content/www/us/en/develop/articles/single-producer-single-consumer-queue.html)
