CC = gcc
CFLAGS = -Wall -g -std=c99 -I$(INCDIR) -L$(LIBDIR)
LIBS = -lm

INCDIR = include/
LIBDIR = lib/
SRCDIR = src/
OBJDIR = obj/
BINDIR = bin/

OUTFILE = main

OBJECTS_BASE = dict dict_item dict_hash
OBJECTS = $(addprefix $(OBJDIR),$(addsuffix .o,$(OBJECTS_BASE)))

.phony: all build clean

all: build

build: $(BINDIR)$(OUTFILE)

$(BINDIR)$(OUTFILE): $(OBJDIR)$(OUTFILE).o $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $< $(OBJECTS) $(LIBS)

$(OBJDIR)%.o: $(SRCDIR)%.c
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	-rm $(BINDIR)*
	-rm $(OBJDIR)*

# additional dependencies

$(SRCDIR)dict.c: $(INCDIR)dict.h $(INCDIR)dict_common.h
$(SRCDIR)dict_item.c: $(INCDIR)dict_item.h $(INCDIR)dict_common.h
$(SRCDIR)dict_hash.c: $(INCDIR)dict_hash.h $(INCDIR)dict_common.h
