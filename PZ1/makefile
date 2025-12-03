CC     = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2
TARGET = zeros_and_randoms
SRC    = main.c
OBJ    = $(SRC:.c=.o)

.PHONY: all clean build post_build

all: build post_build

build: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

post_build:
	-rm -f $(OBJ)

clean:
	-rm -f $(OBJ) $(TARGET)
