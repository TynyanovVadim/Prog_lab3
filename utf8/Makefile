all: main

main.o: src/main.c
	gcc -c -Wall -Werror -o main.o src/main.c

coder.o: src/coder.c
	gcc -c -Wall -Werror -o coder.o src/coder.c

command.o: src/command.c
	gcc -c -Wall -Werror -o command.o src/command.c

main: coder.o command.o main.o
	gcc -Wall -Werror -o main coder.o command.o main.o

.PHONY: clean
clean:
	rm ./main
	rm ./coder.o command.o main.o