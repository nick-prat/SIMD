main: main.o func.o
	gcc -g main.o func.o -o main

main.o: src/main.c
	gcc -c src/main.c -o main.o -g

func.o: src/func.asm
	nasm -f elf64 -o func.o src/func.asm

clean:
	rm func.o main.o main
