AUTOMAKE_OPTIONS = foreign subdir-objects

CFLAGS = -Wall -g -std=c99 -I$(top_srcdir)/include

lib_LIBRARIES = lib/libcdict.a
lib_libcdict_a_SOURCES = src/dict.c src/dict_hash.c src/dict_item.c \
						 include/dict.h include/dict_common.h \
						 include/dict_hash.h include/dict_item.h

include_HEADERS = include/dict.h include/dict_common.h include/dict_hash.h include/dict_item.h
