CC = clang

CFLAGS = -std=c17 -D__USE_MINGW_ANSI_STDIO=1 -g

COPT = -O3 -march=native -mavx2 -mrdrnd -m64

CWARNINGS = -pedantic -Wall -Wextra \
            -Wdouble-promotion -Wnull-dereference -Wconversion \
            -Wcast-qual -Wpacked -Wpadded

program.exe: random_sisd.c random_simd.c random_utils.c random_test.c src/unity.c
	$(CC) $(CFLAGS) $(COPT) $(CWARNINGS) \
    random_utils.c random_sisd.c random_simd.c random_test.c src/unity.c -o program.exe
