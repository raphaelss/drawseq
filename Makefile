FLAGS = -std=c99 -Wall -pedantic -O3 -flto `pkg-config --cflags MagickWand`
LIBS = -lm `pkg-config --libs MagickWand`
CC = gcc
HEADERS = draw.h state.h
EXE = letdraw

letdraw: letdraw.o draw.o state.o
	$(CC) $(FLAGS) letdraw.o draw.o state.o $(LIBS) -o $(EXE)

letdraw.o: letdraw.c
	$(CC) $(FLAGS) -c letdraw.c

draw.o: draw.c draw.h
	$(CC) $(FLAGS) -c draw.c

state.o: state.c state.h
	$(CC) $(FLAGS) -c state.c

clean:
	rm -f *.o $(EXE)
