CC = gcc
CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla -Iinclude
SOURCES = minicat.c src/libopen.c src/libciao.c src/libget.c
OBJETS = src/libopen.o src/libciao.o src/libget.o
TARGET = minicat


minicat: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^

#test: $(SOURCES) src/main.c
#	$(CC) $(CFLAGS) -o -L. -llibstream.a $@ $^

library: $(OBJETS)
	ar csr libstream.a $(OBJETS)

#minicat: minicat.c
#	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)
	rm -rf $(OBJETS)
