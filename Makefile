flags = -Wextra -g3 #-fsanitize=address
cc = gcc

all:
	$(cc) -c src/error.c -o obj/error.o $(flags)
	$(cc) -c src/lexer.c -o obj/lexer.o $(flags)
	$(cc) -c src/parser.c -o obj/parser.o $(flags)
	$(cc) -c main.c -o obj/main.o $(flags)
	$(cc) obj/error.o obj/main.o obj/parser.o obj/lexer.o -o ney $(flags)
