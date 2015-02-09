FLAGS = -std=c99 -Wall -pedantic -O2 `pkg-config --cflags MagickWand`
LIBS = -lm `pkg-config --libs MagickWand`
HEADERS = draw.h position.h
EXE = letdraw

letdraw: letdraw.o draw.o position.o
	cc $(FLAGS) letdraw.o draw.o position.o $(LIBS) -o $(EXE)

letdraw.o: letdraw.c
	cc $(FLAGS) -c letdraw.c

draw.o: draw.c draw.h
	cc $(FLAGS) -c draw.c

position.o: position.c position.h
	cc $(FLAGS) -c position.c

clean:
	rm -f *.o $(EXE)
