all:		test

test:		main.o
			gcc -Wall -o test main.c -lm
			
main.o: 	main.c 
			gcc -Wall -o main.o -c main.c -lm

clean:
			rm -rf *.o

mrproper:	clean
			rm -f test