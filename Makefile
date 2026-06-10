CC=gcc
CFLAGS=-I include -lpthread

utils = utils/constants.c utils/file.c utils/terminal.c utils/strings.c utils/format.c
core = src/core/parse.c src/core/render.c
http = src/http/server.c src/http/request.c src/http/routes.c
app = src/app/routes.c

pulse: main.c $(core) $(utils) $(http) $(app)
	mkdir -p dist
	$(CC) $(CFLAGS) -o dist/pulse main.c $(core) $(utils) $(http) $(app)
