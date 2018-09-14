all: library.c graphics.h snake.c 
	gcc -c library.c snake.c graphics.h
	gcc -o snake library.o snake.o
