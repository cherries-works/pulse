CC=gcc
CFLAGS=-I include -lpthread -Wall -Werror

utils = src/utils/constants.c src/utils/file.c src/utils/terminal.c src/utils/strings.c src/utils/format.c
parse = src/parse/cpu.c src/parse/disk.c src/parse/load.c src/parse/memory.c src/parse/network.c src/parse/process.c src/parse/system.c
http = src/http/server.c src/http/request.c src/http/routes.c
render = src/render/render.c src/render/utils.c
app = src/app/routes.c

pulse: main.c $(render) $(parse) $(utils) $(http) $(app)
	mkdir -p dist
	$(CC) $(CFLAGS) -o dist/pulse main.c $(render) $(parse) $(utils) $(http) $(app)
