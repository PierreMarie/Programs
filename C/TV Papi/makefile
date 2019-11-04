all:		test

test:		main.o
		gcc -o test main.o -lwiringPi

main.o: 	main.c 
		gcc -o main.o -c main.c -lwiringPi

clean:
		rm -rf *.o

mrproper:	clean
		rm -f test