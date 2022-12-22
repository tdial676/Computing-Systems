CC = clang
CFLAGS = -Iinclude -Wall -Wextra -O3 -g

all: bin/cache_timing bin/index_guesser bin/recover_local_secret bin/recover_protected_local_secret bin/exploit

bin/%: src/%.c include/%.h
	$(CC) $(CFLAGS) $(filter-out %.h,$^) -o $@

clean:
	rm -f bin/*

.PRECIOUS: bin/%.o bin/%
