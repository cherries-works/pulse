CC=gcc
INCLUDES=-I "../../include"

progname = shm
bin = dist
target = $(bin)/$(progname)

main = shm.c
utils = ../../src/utils/constants.c
src = $(utils) $(main)

all: $(target)

$(target):	
	mkdir -p $(bin)
	$(CC) $(INCLUDES) $(src) -o $(target)

clean:
	rm -rf $(bin)

.PHONY: all clean
