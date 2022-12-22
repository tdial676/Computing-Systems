CC = clang-with-asan
CFLAGS = -Iinclude -Wall -Wextra -fno-sanitize=integer
ASM = clang

COMPILE_TESTS_1 =                    $(sort $(wildcard progs/stage1-*.bas))
COMPILE_TESTS_2 = $(COMPILE_TESTS_1) $(sort $(wildcard progs/stage2-*.bas))
COMPILE_TESTS_3 = $(COMPILE_TESTS_2) $(sort $(wildcard progs/stage3-*.bas))
COMPILE_TESTS_4 = $(COMPILE_TESTS_3) $(sort $(wildcard progs/stage4-*.bas))
COMPILE_TESTS_5 = $(COMPILE_TESTS_4) $(sort $(wildcard progs/stage5-*.bas))
COMPILE_TESTS_6 = $(COMPILE_TESTS_5) $(sort $(wildcard progs/stage6-*.bas))
COMPILE_TESTS_7 = $(COMPILE_TESTS_6) $(sort $(wildcard progs/stage7-*.bas))

OPT_TESTS_1 = stage7-unhash
OPT_TESTS_2 = stage7-loops-of-ops

all: compile opt1 opt2

compile: compile7
compile1: $(COMPILE_TESTS_1:progs/%.bas=%-result)
compile2: $(COMPILE_TESTS_2:progs/%.bas=%-result)
compile3: $(COMPILE_TESTS_3:progs/%.bas=%-result)
compile4: $(COMPILE_TESTS_4:progs/%.bas=%-result)
compile5: $(COMPILE_TESTS_5:progs/%.bas=%-result)
compile6: $(COMPILE_TESTS_6:progs/%.bas=%-result)
compile7: $(COMPILE_TESTS_7:progs/%.bas=%-result)

opt1: $(OPT_TESTS_1:=-bench)
opt2: $(OPT_TESTS_2:=-bench)

out/%.o: src/%.c
	$(CC) $(CFLAGS) -c $^ -o $@

out/timing.o: runtime/timing.c
	$(ASM) $(CFLAGS) -O3 -c $^ -o $@

bin/compiler: out/ast.o out/compile.o out/compiler.o out/parser.o
	$(CC) $(CFLAGS) $^ -o $@

out/%.s: bin/compiler progs/%.bas
	$^ > $@

bin/%: out/%.s runtime/print_int.s runtime/call_check.s
	$(ASM) -g -nostartfiles $^ -o $@

bin/time-%: out/%.s runtime/print_int_mock.s out/timing.o
	$(ASM) -lm $^ -o $@

progs/%-expected.txt: progs/%.bas
	grep '^#' $^ | sed -e 's/#//' > $@

progs/%-actual.txt: bin/%
	$^ > $@

%-result: progs/%-expected.txt progs/%-actual.txt
	diff -u $^ \
		&& echo PASSED test $(@F:-result=). \
		|| (echo FAILED test $(@F:-result=). Aborting.; false)

progs/%-time.csv: bin/time-%
	$^ > $@

%-bench: compare_times.py reference-times.csv progs/%-time.csv progs/%-speedup.txt
	./$^

clean:
	rm -f out/* bin/* progs/*-expected.txt progs/*-actual.txt progs/*-time.csv

.PRECIOUS: out/%.o out/%.s bin/% bin/time-% \
	progs/%-expected.txt progs/%-actual.txt progs/%-time.csv
