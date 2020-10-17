CC = gcc

CFLAGS = -std=c99 -D__USE_MINGW_ANSI_STDIO=1 -ggdb

COPT = -O3 -march=native -mavx -m64 -mrdrnd

CWARNINGS = -pedantic -Wall -Wextra -Werror \
            -Wdouble-promotion -Wnull-dereference \
            -Wconversion

program: random.c random_test.c src/unity.c
	$(CC) $(CFLAGS) $(COPT) $(CWARNINGS) random.c random_test.c src/unity.c -o program
