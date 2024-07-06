flags = -Wextra -Rsanitize-address
cc = clang

all:
	$(cc) -c src/lexer.c -o obj/lexer.o $(flags)
	$(cc) -c src/parser.c -o obj/parser.o $(flags)
	$(cc) -c main.c -o obj/main.o $(flags)
	$(cc) obj/main.o obj/parser.o obj/lexer.o -o lexer.exe $(flags)
