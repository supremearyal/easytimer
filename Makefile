CC = gcc
SRC = easytimer.c
OUT = easytimer
CFLAGS = -ansi -pedantic -Wall `sdl-config --cflags`
LIBS = `sdl-config --libs` -lSDL_image

all: $(OUT)

debug: $(SRC)
	$(CC) $(CFLAGS) -g -o $(OUT) $(SRC) $(LIBS)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $(OUT) $(SRC) $(LIBS)
