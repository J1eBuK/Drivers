CC     = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = pz1

SRC = main.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)

