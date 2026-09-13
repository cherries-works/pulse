EXIT_NEST = ../..

CC=gcc
LIBS=-lcriterion
INCLUDES=-I$(EXIT_NEST)/include

EXTRA_FLAGS_DEBUG = -fsanitize=address -fno-omit-frame-pointer

FLAGS=-Wall -Werror -Wextra -Wconversion -Wno-unused-parameter -O3
CFLAGS=$(INCLUDES) $(FLAGS)
LDFLAGS=$(LIBS)

test_src = $(wildcard ./test_*.c)
bin = dist
binary = $(bin)/tests

required_src = \
	$(EXIT_NEST)/src/parse/cpu.c \
	$(EXIT_NEST)/src/parse/disk.c \
	$(EXIT_NEST)/src/parse/memory.c \
	$(EXIT_NEST)/src/parse/load.c \
	$(EXIT_NEST)/src/parse/network.c \
	$(EXIT_NEST)/src/utils/strings.c \
	$(EXIT_NEST)/src/utils/constants.c

test: $(binary)
	./$(binary) --verbose

$(binary): $(test_src) $(required_src)
	mkdir -p $(bin)
	$(CC) $(CFLAGS) $(required_src) $(test_src) $(LDFLAGS) -o $(binary)

clean:
	rm -rf $(bin)

.PHONY: all clean
