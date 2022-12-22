#
# Makefile for the malloc lab driver
#

CLEAN_COMMAND = rm -f out/* bin/*
ifdef DEBUG
  CC = clang-with-asan
  CFLAGS = -Werror -Wall -Wextra
  ifeq ($(wildcard .debug),)
    $(shell $(CLEAN_COMMAND))
    $(shell touch .debug)
  endif
else
  CC = clang
  CFLAGS = -Werror -Wall -Wextra -O3 -g
  ifneq ($(wildcard .debug),)
    $(shell $(CLEAN_COMMAND) .debug)
  endif
endif

all: bin/mdriver-implicit bin/mdriver-explicit

bin/mdriver-%: out/mdriver-%.o out/mm-%.o out/memlib.o out/fsecs.o out/fcyc.o out/clock.o
	$(CC) $(CFLAGS) $^ -o $@

out/mdriver-implicit.o: driver/mdriver.c
	$(CC) $(CFLAGS) -c -DSTAGE0 $^ -o $@

out/mdriver-explicit.o: driver/mdriver.c
	$(CC) $(CFLAGS) -c -DSTAGE1 $^ -o $@

out/%.o: src/%.c
	$(CC) $(CFLAGS) -Iinclude -c $^ -o $@

out/%.o: driver/%.c
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	$(CLEAN_COMMAND)

.PRECIOUS: bin/mdriver-% out/mdriver-implicit.o out/mdriver-explicit.o out/%.o
