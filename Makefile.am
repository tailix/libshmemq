SUBDIRS = include

AM_CFLAGS = \
	-std=c99 \
	-Wall \
	-Wextra \
	-I$(top_srcdir)/include

lib_LIBRARIES = libshmemq.a

TESTS = \
	tests/test_alternatively_push_and_pop_one_frame_messages_with_jump \
	tests/test_cons_pops_buffer_start \
	tests/test_cons_reaches_queue_end \
	tests/test_error_str \
	tests/test_fork \
	tests/test_main \
	tests/test_prod_jumps_to_buffer_start_and_pushes_too_long_message \
	tests/test_prod_jumps_to_buffer_start_and_reaches_cons \
	tests/test_prod_pushes_too_long_message \
	tests/test_prod_reaches_buffer_end \
	tests/test_push_and_pop_one_frame_messages_without_jump

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

tests_test_alternatively_push_and_pop_one_frame_messages_with_jump_SOURCES = \
	$(libshmemq_a_SOURCES) \
	tests/test_alternatively_push_and_pop_one_frame_messages_with_jump.c

tests_test_cons_pops_buffer_start_SOURCES = \
	$(libshmemq_a_SOURCES) \
	tests/test_cons_pops_buffer_start.c

tests_test_cons_reaches_queue_end_SOURCES = \
	$(libshmemq_a_SOURCES) \
	tests/test_cons_reaches_queue_end.c

tests_test_error_str_SOURCES = \
	$(libshmemq_a_SOURCES) \
	tests/test_error_str.c

tests_test_fork_SOURCES = \
	$(libshmemq_a_SOURCES) \
	tests/test_fork.c

tests_test_main_SOURCES = \
	$(libshmemq_a_SOURCES) \
	tests/test_main.c

tests_test_prod_jumps_to_buffer_start_and_pushes_too_long_message_SOURCES = \
	$(libshmemq_a_SOURCES) \
	tests/test_prod_jumps_to_buffer_start_and_pushes_too_long_message.c

tests_test_prod_jumps_to_buffer_start_and_reaches_cons_SOURCES = \
	$(libshmemq_a_SOURCES) \
	tests/test_prod_jumps_to_buffer_start_and_reaches_cons.c

tests_test_prod_pushes_too_long_message_SOURCES = \
	$(libshmemq_a_SOURCES) \
	tests/test_prod_pushes_too_long_message.c

tests_test_prod_reaches_buffer_end_SOURCES = \
	$(libshmemq_a_SOURCES) \
	tests/test_prod_reaches_buffer_end.c

tests_test_push_and_pop_one_frame_messages_without_jump_SOURCES = \
	$(libshmemq_a_SOURCES) \
	tests/test_push_and_pop_one_frame_messages_without_jump.c
