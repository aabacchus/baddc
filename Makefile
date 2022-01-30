.POSIX:

BIN = baddc
SRCS = baddc.c
XCFLAGS = -Wall -Wextra -pedantic -g

$(BIN): $(SRCS)
	$(CC) $(XCFLAGS) -o $(BIN) $(SRCS)

clean:
	rm -f $(BIN)

.PHONY: clean
