all: cuisinier serveur

cuisinier: cook.o common.o
	gcc cook.o common.o -o cuisinier -lrt -pthread

serveur: waiter.o common.o
	gcc waiter.o common.o -o serveur -lrt -pthread

cook.o: cook.c common.h
	gcc -Wall -c cook.c -o cook.o -lrt -pthread

waiter.o: waiter.c common.h
	gcc -Wall -c waiter.c -o waiter.o -lrt -pthread

common.o: common.c
	gcc -Wall -c common.c -o common.o

clean:
	rm -rf *.o cuisinier serveur
