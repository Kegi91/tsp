SRC_FILES=main.c rng.c SFMT-src-1.5.1/SFMT.c

CFLAGS+=-Wall -Wextra -pedantic -Wno-maybe-uninitialized
CFLAGS+=-g -std=c99 -DSFMT_MEXP=607 -lm -O3

main: $(SRC_FILES)
	gcc $(CFLAGS) -o $@ $(SRC_FILES)

clean:
	rm -f main

run: main
	./main 2000 1000000 10
	python3 plot.py
