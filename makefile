main: main.o func.o
	gcc -g main.o func.o -lpng -lz -o main

main.o: src/main.c
	gcc -g -c src/main.c -o main.o

func.o: src/func.asm
	nasm -f elf64 -o func.o src/func.asm

clean:
	rm func.o main.o main
