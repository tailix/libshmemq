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
 ___ ___ ___ ___ ___ ___ _V_ ___ ___ ___
|   |   |13 |14 |15 |16 |   |   |   |   |
|___|___|___|___|___|___|___|___|___|___|
          |
          V
```

Consumer pops more:

```
                          |
 ___ ___ ___ ___ ___ ___ _V_ ___ ___ ___
|   |   |   |   |   |16 |   |   |   |   |
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
