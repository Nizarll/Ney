CC = clang
CFLAGS = -Wall -I ./libs -std=c23 -g3  -Ddebug

sources = src/main.c src/ast.c src/lexer.c src/parser.c src/allocators.c
target = bin/ney
objects_dir = obj
objects = $(sources:src/%.c=$(objects_dir)/%.o)

all: $(target)

clean:
	rm -rf $(objects_dir)

$(target): $(objects)
	$(CC) $(CFLAGS) -o $(target) $(objects)

$(objects_dir)/%.o: src/%.c | $(objects_dir)
	$(CC) $(CFLAGS) -c $< -o $@

$(objects_dir):
	mkdir -p $(objects_dir)
