CC = clang-with-asan
CFLAGS = -Wall -Wextra -Werror -fno-sanitize=integer
TESTS_1 = OnePlusTwo
TESTS_2 = $(TESTS_1) PrintOnePlusTwo
TESTS_3 = $(TESTS_2) Constants Part3
TESTS_4 = $(TESTS_3) Part4 Bitmasks Bitshifts
TESTS_5 = $(TESTS_4) Locals MoreLocals
TESTS_6 = $(TESTS_5) PrintLargeNumbers
TESTS_7 = $(TESTS_6) Collatz PythagoreanTriplet
TESTS_8 = $(TESTS_7) Arithmetic CoinSums DigitPermutations FunctionCall \
	Goldbach IntegerTypes BitwiseFunctions Jumps PalindromeProduct Primes Recursion
TESTS_9 = $(TESTS_8) IntArraysPart1 IntArraysPart2 IntArraysPart3 IntArraysPart4 \
	IntArraysPart5 CoinSumsAlternate MergeSort SieveOfErathosthenes

test: test9
test1: $(TESTS_1:=-result)
test2: $(TESTS_2:=-result)
test3: $(TESTS_3:=-result)
test4: $(TESTS_4:=-result)
test5: $(TESTS_5:=-result)
test6: $(TESTS_6:=-result)
test7: $(TESTS_7:=-result)
test8: $(TESTS_8:=-result)
test9: $(TESTS_9:=-result)

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

jvm: jvm.o read_class.o heap.o
	$(CC) $(CFLAGS) $^ -o $@

tests/%.class: tests/%.java
	javac $^

tests/%-expected.txt: tests/%.class
	java -cp tests $(*F) > $@

tests/%-actual.txt: tests/%.class jvm
	./jvm $< > $@

%-result: tests/%-expected.txt tests/%-actual.txt
	diff -u $^ \
		&& echo PASSED test $(@:-result=). \
		|| (echo FAILED test $(@:-result=). Aborting.; false)

clean:
	rm -f *.o jvm tests/*.txt `find tests -name '*.java' | sed 's/java/class/'`

.PRECIOUS: %.o tests/%.class tests/%-expected.txt tests/%-actual.txt tests/%-result.txt
