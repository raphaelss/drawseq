FLAGS = -std=c99 -Wall -pedantic -g `pkg-config --cflags cairo`
LIBS = -lm `pkg-config --libs cairo`
CC = gcc
EXE = drawseq

drawseq: drawseq.c draw.o
	$(CC) $(FLAGS) drawseq.c draw.o $(LIBS) -o $(EXE)

draw.o: draw.c draw.h
	$(CC) $(FLAGS) -c draw.c

clean:
	rm -f *.o $(EXE)

