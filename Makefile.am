SUBDIRS = include

AM_CFLAGS = \
	-std=c99 \
	-Wall \
	-Wextra \
	-I$(top_srcdir)/include

lib_LIBRARIES = libshmemq.a

TESTS = \
	tests/test_finish_consumer \
	tests/test_finish_producer \
	tests/test_init_consumer \
	tests/test_init_producer

noinst_PROGRAMS = \
	$(TESTS) \
	examples/raw_receiver \
	examples/raw_sender

libshmemq_a_SOURCES = src/main.c

examples_raw_receiver_SOURCES = \
	$(libshmemq_a_SOURCES) \
	examples/raw_receiver.c

examples_raw_sender_SOURCES = \
	$(libshmemq_a_SOURCES) \
	examples/raw_sender.c

tests_test_finish_consumer_SOURCES = \
	$(libshmemq_a_SOURCES) \
	tests/test_finish_consumer.c

tests_test_finish_producer_SOURCES = \
	$(libshmemq_a_SOURCES) \
	tests/test_finish_producer.c

tests_test_init_consumer_SOURCES = \
	$(libshmemq_a_SOURCES) \
	tests/test_init_consumer.c

tests_test_init_producer_SOURCES = \
	$(libshmemq_a_SOURCES) \
	tests/test_init_producer.c
