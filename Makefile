CC=gcc
CFLAGS=-Wall -Wextra
LDFLAGS=`pkg-config --libs raylib` -lm
OUTPATH=ventrella

SRC=$(wildcard *.c)
OBJ=$(patsubst %.c,build/%.o,$(SRC))

all: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(OUTPATH) $(LDFLAGS)

build/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -o $@ $< -c $(CFLAGS)

clean:
	rm -rf $(OUTPATH) build/
