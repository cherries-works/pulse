CC=gcc
LIBS=-lpthread
INCLUDES=-Iinclude
FLAGS=-Wall -Werror -Wconversion -Wno-unused-parameter
CFLAGS=$(INCLUDES) $(FLAGS)
LDFLAGS=$(LIBS)

progname = pulse
build = build
bin = dist
target = $(bin)/$(progname)

main = main.c
utils = src/utils/constants.c src/utils/file.c src/utils/terminal.c src/utils/strings.c src/utils/format.c src/utils/args.c
parse = src/parse/cpu.c src/parse/disk.c src/parse/load.c src/parse/memory.c src/parse/network.c src/parse/process.c src/parse/system.c
http = src/http/server.c src/http/request.c src/http/routes.c
render = src/render/render.c src/render/utils.c src/render/start.c
daemon = src/daemon/start.c
log = src/log/start.c
app = src/app/routes.c src/app/start.c
setup = src/setup/setup.c

src = $(render) $(parse) $(utils) $(http) $(setup) $(daemon) $(log) $(app) $(main)

obj = $(src:%.c=$(build)/%.o)
dep = $(obj:.o=.d)

all: $(target)

$(target): $(obj) | $(bin)
	$(CC) $(obj) -o $(target) $(LDFLAGS)

# any .c file, compile it into .o file in build folder
# $@ is the output file, like build/src/utils/file.o
#
# @ is to NOT print the command.
# $< (the .c file) | $@ (already mentioned) .o file.
$(build)/%.o: %.c | $(build)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# to rebuild on header changes
-include $(dep)

$(build):
	mkdir -p $(build)

$(bin):
	mkdir -p $(bin)

clean:
	rm -rf $(build) $(bin)

.PHONY: all clean
