main: main.o func.o
	gcc -g main.o func.o -o main

main.o: main.c
	gcc -c main.c -o main.o -g

func.o: func.asm
	nasm -f elf64 -o func.o func.asm

clean:
	rm func.o main.o