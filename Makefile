CC = clang

all: main

main:
	${CC} src/main.c -o target/main -lpthread

.PHONY clean:
clean:
	rm target/*