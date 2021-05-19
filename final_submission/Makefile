CC=gcc
CFLAGS=-fsanitize=address -Wall  -std=gnu11 -g -lm

tests: tests.c virtual_alloc.c
	$(CC) $(CFLAGS) $^ -o $@ -L"." -lcmocka-static
run_tests:
	./tests