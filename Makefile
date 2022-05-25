.POSIX:

XCFLAGS = $(CFLAGS) -Wall -Wextra -pedantic -Wno-array-bounds -g

baddc: baddc.c
	$(CC) $(XCFLAGS) -o $@ baddc.c -lm

clean:
	rm -f baddc
