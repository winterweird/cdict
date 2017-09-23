CC = gcc
AR = ar
ARFLAGS = -crv
CFLAGS = -Wall -g -std=c99 -I$(INCDIR) -L$(LIBDIR)
LIBS = -lm -lcdict

INCDIR = include/
LIBDIR = lib/
SRCDIR = src/
OBJDIR = obj/
BINDIR = bin/

OUTFILE = main

OBJECTS_BASE = dict dict_item dict_hash
OBJECTS = $(addprefix $(OBJDIR),$(addsuffix .o,$(OBJECTS_BASE)))

LIBOUT = $(LIBDIR)libcdict.a

.phony: all build clean library

all: init build

build: $(BINDIR)$(OUTFILE)

library: $(LIBOUT)

$(BINDIR)$(OUTFILE): $(OBJDIR)$(OUTFILE).o library
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

$(OBJDIR)%.o: $(SRCDIR)%.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(LIBOUT): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

clean:
	-rm $(BINDIR)*
	-rm $(OBJDIR)*
	-rm $(LIBDIR)*

init:
	-mkdir src include lib bin obj

# additional dependencies

$(SRCDIR)dict.c: $(INCDIR)dict.h $(INCDIR)dict_common.h
$(SRCDIR)dict_item.c: $(INCDIR)dict_item.h $(INCDIR)dict_common.h
$(SRCDIR)dict_hash.c: $(INCDIR)dict_hash.h $(INCDIR)dict_common.h
