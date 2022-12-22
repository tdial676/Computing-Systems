CLEAN_COMMAND = rm -f bin/* passwords.txt tests/*-actual*.txt
ifdef DEBUG
  CC = clang
  CFLAGS = -Iinclude -Wall -Wextra -fsanitize=address,undefined
  ifeq ($(wildcard .debug),)
    $(shell $(CLEAN_COMMAND))
    $(shell touch .debug)
  endif
else
  CC = clang
  CFLAGS = -Iinclude -Wall -Wextra -O3 -g
  ifneq ($(wildcard .debug),)
    $(shell $(CLEAN_COMMAND) .debug)
  endif
endif

TESTS_SQUEUE = squeue_single_fill squeue_push_pop
TESTS_MQUEUE = mqueue_push_pop mqueue_empty mqueue_multiple_queues
PRIMES_THREADS = 1 2 4 8 16 32 64
SLEEPERS = 10
SLEEPERS_THREADS=$(shell seq 1 $(SLEEPERS))
RECURSIVE_WORK = 3125
TESTS_THREADPOOL = $(PRIMES_THREADS:%=prime_printer-%) primes_multiple_pools \
	primes_repeat_drain primes_periodic_work recursive_add_work \
	$(SLEEPERS_THREADS:%=sleepers-%)

test: test_queue test_threadpool
test_queue: test_squeue test_mqueue

test_squeue: $(TESTS_SQUEUE:=-result)
	@echo "\e[32mALL SINGLE-THREADED QUEUE TESTS PASS!\e[39m"

test_mqueue: $(TESTS_MQUEUE:=-result)
	@echo "\e[32mALL MULTI-THREADED QUEUE TESTS PASS!\e[39m"

test_threadpool: $(TESTS_THREADPOOL:=-result)
	@echo "\e[32mALL THREADPOOL TESTS PASS!\e[39m"

passwords.txt: bin/password_cracker hashes.txt
	nohup stdbuf -oL $< < hashes.txt > $@ 2>&1

bin/%.o: src/%.c
	$(CC) $(CFLAGS) -c $^ -o $@

bin/%.o: tests/%.c
	$(CC) $(CFLAGS) -c $^ -o $@

bin/%: bin/%.o bin/queue.o bin/thread_pool.o
	$(CC) $(CFLAGS) -lpthread $^ -o $@

bin/password_cracker: bin/password_cracker.o bin/queue.o bin/thread_pool.o
	$(CC) $(CFLAGS) -lcrypt -lpthread $^ -o $@

mqueue_push_pop-result: tests/mqueue_push_pop-actual-sorted.txt
	diff -u tests/correct_integers.txt $^ \
		&& echo "\e[32mPASSED test mqueue_push_pop\e[39m" \
		|| (echo "\e[31mFAILED test mqueue_push_pop\e[39m"; false)

tests/prime_printer-%-actual.txt: bin/prime_printer
	$^ $(@:tests/prime_printer-%-actual.txt=%) > $@

prime_printer-%-result: tests/prime_printer-%-actual-sorted.txt
	diff -u tests/correct_primes.txt $^ \
		&& echo "\e[32mPASSED test prime_printer with $(@:prime_printer-%-result=%) threads\e[39m" \
		|| (echo "\e[31mFAILED test prime_printer with $(@:prime_printer-%-result=%) threads\e[39m"; false)

primes_%-result: tests/primes_%-actual-sorted.txt
	diff -u tests/correct_primes.txt $^ \
		&& echo "\e[32mPASSED test $(@:-result=)\e[39m" \
		|| (echo "\e[31mFAILED test $(@:-result=)\e[39m"; false)

recursive_add_work-result: tests/recursive_add_work-actual-lines.txt
	actual=`cat $^`; \
	[ $$actual = $(RECURSIVE_WORK) ] \
		&& echo "\e[32mPASSED test recursive_add_work\e[39m" \
		|| (echo "\e[31mFAILED test recursive_add_work"; \
			echo "Expected $(RECURSIVE_WORK) prints but found $$actual prints\e[39m"; false)

tests/sleepers-%-actual.txt: bin/sleepers
	$^ $(@:tests/sleepers-%-actual.txt=%) > $@

sleepers-%-result: tests/sleepers-%-actual.txt
	actual=`cat $^`; \
	expected=$$((($(SLEEPERS) + $(@:sleepers-%-result=%) - 1) / $(@:sleepers-%-result=%))); \
	[ $$actual = $$expected ] \
		&& echo "\e[32mPASSED test sleepers with $(@:sleepers-%-result=%) threads\e[39m" \
		|| (echo "\e[31mFAILED test sleepers with $(@:sleepers-%-result=%) threads"; \
			echo "Expected to sleep $$expected seconds, but slept $$actual seconds\e[39m"; false)

%-result: bin/%
	$^ && echo "\e[32mPASSED test $(@:-result=)\e[39m" \
		|| (echo "\e[31mFAILED test $(@:-result=)\e[39m"; false)

tests/%-actual.txt: bin/%
	$^ > $@

tests/%-sorted.txt: tests/%.txt
	sort -n $^ > $@

tests/%-lines.txt: tests/%.txt
	wc -l < $^ > $@

clean:
	$(CLEAN_COMMAND)

.PRECIOUS: passwords.txt bin/%.o bin/% bin/password_cracker \
	tests/prime_printer-%-actual.txt tests/sleepers-%-actual.txt \
	tests/%-actual.txt tests/%-sorted.txt tests/%-lines.txt
