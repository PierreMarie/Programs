all:		test

test:		main.o
			gcc -lm -o test main.o -lpthread

main.o:	main.c 
			gcc -lm -o main.o -c main.c -lpthread

clean:
			rm -rf *.o

mrproper:clean
			rm -f test