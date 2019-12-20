CC = gcc

main : main.o LectureArg.o hashage.o
	$(CC) -Wall main.o LectureArg.o hashage.o -o main -l ssl -l crypto
main.o : main.c
	$(CC) -c main.c -o main.o
LectureArg.o : LectureArg.c LectureArg.h
	$(CC) -c LectureArg.c -o LectureArg.o
hashage.o : hashage.c hashage.h
	$(CC) -c hashage.c -o hashage.o
