# Author:   Debajyoti Nandi <debajyoti.nandi@gmail.com>
# Created:  2016-07-09
# Modified: 2016-11-30
# License:  MIT License (See LICENSE.txt)
#
# Note: Update the "headers", "object files" and the "executable"
# 	sections as needed.
#
# See:
# http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

# Compiler:
CLANG = clang-3.6
GCC = gcc
CC = $(GCC)
#CC = $(CLANG)

# Compiler flags:
CFLAGS = -std=gnu11 -Ofast #-DDEBUG

# Include directory:
IDIR = .

# Library directory:
LDIR = .

# Object directory:
ODIR = .

# Libraries
LIBS = -lpthread

# Headers
_DEPS = util.h qseries.h partition.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

# Object files:
_OBJS = util.o qseries.o partition.o
OBJS = $(patsubst %,$(ODIR)/%$(_OBJS))

# Executables:
EXES = genpartn partnid

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

all: $(EXES)

genpartn: genpartn.c partition.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	@echo "---> Successfully compiled executable: genpartn*"

partnid: partnid.c partition.o qseries.o util.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	@echo "---> Successfully compiled executable: partnid*"

.PHONY: clean

clean:
	$(RM) $(ODIR)/*.o

distclean: clean
	$(RM) $(EXES)
