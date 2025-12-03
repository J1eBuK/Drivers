CC     = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2
TARGET = zeros_and_randoms
SRC    = main.c
OBJ    = $(SRC:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

post_build:
	-rm -f $(OBJ)

clean:
	@echo "Cleaning everything..."
	-rm -f $(OBJ) $(TARGET)
