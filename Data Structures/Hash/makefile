CC = gcc
CFLAGS = -Werror -Wall -Wextra -pedantic -ggdb -std=c99 -DUNITY_INCLUDE_DOUBLE

program: hash.c hash.h test_hash.c src/unity.c
	$(CC) -o program hash.c test_hash.c src/unity.c $(CFLAGS)