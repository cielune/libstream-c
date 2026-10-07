CC = gcc
CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla -Iinclude
SRCES = src/libopen.c src/libclose.c src/libget.c
OBJECTS = $(SRCES:.c=.o)


all: libstream.a demo demo-full

libstream.a: $(OBJECTS)
	ar csr $@ $^

demo-full: fullmain.c libstream.a
	$(CC) $(CFLAGS) -o $@ $< -L. -lstream

demo: main.c libstream.a
	$(CC) $(CFLAGS) -o $@ $< -L. -lstream

clean:
	rm -f $(OBJECTS) libstream.a demo demo-full demo-full.txt demo.txt
