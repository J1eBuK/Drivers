CC     = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2
TARGET = pz1

SRC = main.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)
	rm -f $(TARGET)
	$(CC) $(CFLAGS) -c $(SRC) -o $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)
	-rm -f $(OBJ)

clean:
	rm -f $(OBJ) $(TARGET)

