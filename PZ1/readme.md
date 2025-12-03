# Задача №1
Написать программу для чтения 100 байт с устройств `/dev/zero` и `/dev/random`

## `makefile` для сборки
```makefile
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
```
## `main.c` - программа для чтения данных

```c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int read_zero() {
    int fd = open("/dev/zero", O_RDONLY);
    if (fd < 0) {
        perror("open /dev/zero");
        return -1;
    }

    unsigned char buf[100];
    int n = read(fd, buf, sizeof(buf));
    if (n < 0) {
        perror("read /dev/zero");
        close(fd);
        return -1;
    }

    printf("Read %d bytes from /dev/zero:\n", n);
    for (int i = 0; i < n; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n\n");

    close(fd);
    return 0;
}

int read_random() {
    int fd = open("/dev/random", O_RDONLY);
    if (fd < 0) {
        perror("open /dev/random");
        return -1;
    }

    unsigned char buf[100];
    int n = read(fd, buf, sizeof(buf));
    if (n < 0) {
        perror("read /dev/random");
        close(fd);
        return -1;
    }

    printf("Read %d bytes from /dev/random:\n", n);
    for (int i = 0; i < n; i++) {
        printf("%02X ", buf[i]);
    }
    printf("\n\n");

    close(fd);
    return 0;
}

int main() {
    printf("=== Reading /dev/zero ===\n");
    if (read_zero() < 0)
        return 1;

    printf("=== Reading /dev/random ===\n");
    if (read_random() < 0)
        return 1;

    return 0;
}
```
### Пример вывода при выполнении программы:
```
dudka@DVM:~/Drivers/PZ1$./zeros_and_randoms
=== Reading /dev/zero ===
Read 100 bytes from /dev/zero:
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
=== Reading /dev/random ===
Read 100 bytes from /dev/random:
4E 2D 5B A8 C8 F8 67 DE 62 F1 63 OF 6D FC OF 5D B9 7C 4F 75 34 DF 8E E7 5F EB 1F 60 2B FE 19 F5 8C 61 92 8E 68 AF 08 98 9E 3B D6 D3 83 89 FA 23 09 3B 96 57 3B 5 7 10 76 05 A0 F7 01 C4 F2 A9 54 08 СЕ 82 62 FE 0A DF 03 28 13 B7 0F 61 77 F3 D2 83 A4 B1 2E AF 24 CD 3B D5 B1 2A 44 36 96 3С ЕЕ СВ В4 5A 59
```

# Задача №2

При помощи утилиты strace отследить системные вызовы при работе приложения.

Вызов утилиты:

```
strace -o trace.log ./zeroes_and_randoms
cp trace.log trace.txt
```

Вывод вызова утилиты strace представлен в файле trace.txt.
