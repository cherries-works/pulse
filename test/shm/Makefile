CC=gcc
INCLUDES=-I "../../include"

progname = test_shm
bin = dist
target = $(bin)/$(progname)

main = test_shm.c
utils = ../../src/utils/constants.c
src = $(utils) $(main)

all: $(target)

$(target):	
	mkdir -p $(bin)
	$(CC) $(INCLUDES) $(src) -o $(target)

clean:
	rm -rf $(bin)

.PHONY: all clean
