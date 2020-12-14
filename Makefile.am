SUBDIRS = include

AM_CFLAGS = \
	-std=c99 \
	-Wall \
	-Wextra \
	-I$(top_srcdir)/include

lib_LIBRARIES = libshmemq.a

TESTS = \
	tests/test_main \
	tests/test_cons_pops_buffer_start \
	tests/test_cons_reaches_queue_end \
	tests/test_prod_jumps_to_buffer_start_and_reaches_cons \
	tests/test_prod_reaches_buffer_end

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

tests_test_main_SOURCES = \
	$(libshmemq_a_SOURCES) \
	tests/test_main.c

tests_test_cons_pops_buffer_start_SOURCES = \
	$(libshmemq_a_SOURCES) \
	tests/test_cons_pops_buffer_start.c

tests_test_cons_reaches_queue_end_SOURCES = \
	$(libshmemq_a_SOURCES) \
	tests/test_cons_reaches_queue_end.c

tests_test_prod_jumps_to_buffer_start_and_reaches_cons_SOURCES = \
	$(libshmemq_a_SOURCES) \
	tests/test_prod_jumps_to_buffer_start_and_reaches_cons.c

tests_test_prod_reaches_buffer_end_SOURCES = \
	$(libshmemq_a_SOURCES) \
	tests/test_prod_reaches_buffer_end.c
