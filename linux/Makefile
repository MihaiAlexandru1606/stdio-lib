CC = gcc
CFLAGS = -Wall -fpic -Werror -Wextra
LDFLAGS = -shared
TARGET = libso_stdio.so

SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OBJECTS = $(SOURCES:.c=.o)

build: $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC)  $(LDFLAGS) -o $@ $(OBJECTS)

.PHONY: clean
clean:
	rm -fr $(OBJECTS) $(TARGET)
