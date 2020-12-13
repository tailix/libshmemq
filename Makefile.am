SUBDIRS = include

AM_CFLAGS = \
	-std=c99 \
	-Wall \
	-Wextra \
	-I$(top_srcdir)/include

lib_LIBRARIES = libshmemq.a

TESTS = \
	tests/test_dummy \
	tests/test_init

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

tests_test_dummy_SOURCES = \
	$(libshmemq_a_SOURCES) \
	tests/test_dummy.c

tests_test_init_SOURCES = \
	$(libshmemq_a_SOURCES) \
	tests/test_init.c
