CC = clang-with-asan
CFLAGS = -Iinclude -Wall -Wextra

TESTS = $(sort $(patsubst tests/%-input.txt,%-result,$(wildcard tests/*-input.txt)))

all: test recovery

out/%.o: src/%.c
	$(CC) $(CFLAGS) -c $^ -o $@

bin/test_tree: out/test_tree.o out/directory_tree.o
	$(CC) $(CFLAGS) $^ -o $@

bin/recover: out/recover.o out/fat16.o out/directory_tree.o
	$(CC) $(CFLAGS) $^ -o $@

tests/%-actual.txt tests/%-actual-files: bin/test_tree tests/%-input.txt
	rm -rf $(@:.txt=-files)
	$^ $(@:.txt=-files) > $(@:-files=.txt)

tests/%-expected-files: tests/%-expected-files.tar.gz
	rm -rf $@
	tar -xzf $^ -C $(@D)

%-result: tests/%-expected.txt tests/%-actual.txt tests/%-expected-files tests/%-actual-files
	diff -u $(wordlist 1,2,$^) && diff -ur $(wordlist 3,4,$^) \
		&& echo PASSED test $(@:-result=). \
		|| (echo FAILED test $(@:-result=). Aborting.; false)

test: $(TESTS)

usb.dmg:
	curl "https://com.puter.systems/22fa/projects/assets/00/usb.dmg" -o $@

recovery: bin/recover usb.dmg
	rm -rf $@
	mkdir $@
	cd $@ && $(patsubst %,../%,$^)

clean:
	rm -rf out/* bin/* tests/*-actual* tests/*-expected-files usb.dmg recovery
	touch bin/leaks.supp

.PRECIOUS: out/%.o bin/test_tree bin/recover \
	tests/%-actual.txt tests/%-actual-files tests/%-expected-files \
	usb.dmg recovery
