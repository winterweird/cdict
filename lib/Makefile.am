AUTOMAKE_OPTIONS = subdir-objects

AM_CFLAGS = -Wall -g -std=c99 -I$(top_srcdir)/include

lib_LIBRARIES = libcdict.a
libcdict_a_SOURCES = $(top_srcdir)/src/dict.c \
		     $(top_srcdir)/src/dict_hash.c \
		     $(top_srcdir)/src/dict_item.c
