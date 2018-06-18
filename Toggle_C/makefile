all:		test

test:		main.o
		gcc -o test main.o -lwiringPi -lpthread

main.o: 	main.c 
		gcc -o main.o -c main.c -lwiringPi -lpthread

clean:
		rm -rf *.o

mrproper:	clean
		rm -f test