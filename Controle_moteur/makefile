all:		test

test:		main.o
			gcc -o test main.o -Wall -lwiringPi -lpthread

main.o: 	main.c 
			gcc -o main.o -c main.c -Wall -lwiringPi -lpthread

clean:
			rm -rf *.o

mrproper:	clean
			rm -f test