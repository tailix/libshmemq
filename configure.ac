AC_PREREQ([2.68])
AC_INIT([libshmemq],
  [0.0.0],
  [https://github.com/kotovalexarian/libshmemq/issues],
  [libshmemq],
  [https://github.com/kotovalexarian/libshmemq])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([src/main.c])

AC_CANONICAL_HOST

AM_INIT_AUTOMAKE([1.9 subdir-objects])

AC_CONFIG_FILES([
  Makefile
  include/Makefile
])

AC_LANG([C])

AM_PROG_AR
AC_PROG_CC
AC_PROG_CC_C99
AC_PROG_RANLIB

AC_TYPE_SIZE_T

AC_FUNC_MALLOC
AC_FUNC_MMAP

AC_CHECK_HEADER_STDBOOL
AC_CHECK_HEADERS([fcntl.h])
AC_CHECK_HEADERS([stddef.h])
AC_CHECK_HEADERS([stdlib.h])
AC_CHECK_HEADERS([string.h])
AC_CHECK_HEADERS([sys/mman.h])
AC_CHECK_HEADERS([sys/stat.h])

AC_CHECK_FUNCS([ftruncate])
AC_CHECK_FUNCS([munmap])
AC_CHECK_FUNCS([strcpy])
AC_CHECK_FUNCS([strlen])

AC_SEARCH_LIBS([shm_open], [rt])
AC_SEARCH_LIBS([shm_unlink], [rt])

AC_OUTPUT
