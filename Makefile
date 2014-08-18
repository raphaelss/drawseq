FLAGS = -std=c99 -Wall -pedantic -O3 -flto `pkg-config --cflags cairo`
LIBS = -lm `pkg-config --libs cairo`
CC = gcc
EXE = letdraw

letdraw: letdraw.o draw.o
	$(CC) $(FLAGS) letdraw.o draw.o $(LIBS) -o $(EXE)

letdraw.o: letdraw.c
	$(CC) $(FLAGS) -c letdraw.c

draw.o: draw.c draw.h
	$(CC) $(FLAGS) -c draw.c

clean:
	rm -f *.o $(EXE)

