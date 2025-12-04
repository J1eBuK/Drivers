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
## `main.c` — программа для чтения данных с /dev/zero и /dev/random

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

```
execve("./zeros_and_randoms", ["./zeros_and_randoms"], 0x7ffd65e94d90 /* 29 vars */) = 0
brk(NULL)                               = 0x5651dea03000
mmap(NULL, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x713c8b697000
access("/etc/ld.so.preload", R_OK)      = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/etc/ld.so.cache", O_RDONLY|O_CLOEXEC) = 3
fstat(3, {st_mode=S_IFREG|0644, st_size=33283, ...}) = 0
mmap(NULL, 33283, PROT_READ, MAP_PRIVATE, 3, 0) = 0x713c8b68e000
close(3)                                = 0
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libc.so.6", O_RDONLY|O_CLOEXEC) = 3
read(3, "\177ELF\2\1\1\3\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\220\243\2\0\0\0\0\0"..., 832) = 832
pread64(3, "\6\0\0\0\4\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0"..., 784, 64) = 784
fstat(3, {st_mode=S_IFREG|0755, st_size=2125328, ...}) = 0
pread64(3, "\6\0\0\0\4\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0"..., 784, 64) = 784
mmap(NULL, 2170256, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x713c8b400000
mmap(0x713c8b428000, 1605632, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x28000) = 0x713c8b428000
mmap(0x713c8b5b0000, 323584, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1b0000) = 0x713c8b5b0000
mmap(0x713c8b5ff000, 24576, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1fe000) = 0x713c8b5ff000
mmap(0x713c8b605000, 52624, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0) = 0x713c8b605000
close(3)                                = 0
mmap(NULL, 12288, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x713c8b68b000
arch_prctl(ARCH_SET_FS, 0x713c8b68b740) = 0
set_tid_address(0x713c8b68ba10)         = 1196
set_robust_list(0x713c8b68ba20, 24)     = 0
rseq(0x713c8b68c060, 0x20, 0, 0x53053053) = 0
mprotect(0x713c8b5ff000, 16384, PROT_READ) = 0
mprotect(0x5651bd36b000, 4096, PROT_READ) = 0
mprotect(0x713c8b6cf000, 8192, PROT_READ) = 0
prlimit64(0, RLIMIT_STACK, NULL, {rlim_cur=8192*1024, rlim_max=RLIM64_INFINITY}) = 0
munmap(0x713c8b68e000, 33283)           = 0
fstat(1, {st_mode=S_IFCHR|0600, st_rdev=makedev(0x4, 0x1), ...}) = 0
ioctl(1, TCGETS, {c_iflag=BRKINT|ICRNL|IXON|IMAXBEL|IUTF8, c_oflag=NL0|CR0|TAB0|BS0|VT0|FF0|OPOST|ONLCR, c_cflag=B38400|CS8|CREAD|HUPCL, c_lflag=ISIG|ICANON|ECHO|ECHOE|ECHOK|IEXTEN|ECHOCTL|ECHOKE, ...}) = 0
getrandom("\xde\x4c\xec\x1d\x77\x15\x2c\xba", 8, GRND_NONBLOCK) = 8
brk(NULL)                               = 0x5651dea03000
brk(0x5651dea24000)                     = 0x5651dea24000
write(1, "=== Reading /dev/zero ===\n", 26) = 26
openat(AT_FDCWD, "/dev/zero", O_RDONLY) = 3
read(3, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"..., 100) = 100
write(1, "Read 100 bytes from /dev/zero:\n", 31) = 31
write(1, "00 00 00 00 00 00 00 00 00 00 00"..., 302) = 302
close(3)                                = 0
write(1, "=== Reading /dev/random ===\n", 28) = 28
openat(AT_FDCWD, "/dev/random", O_RDONLY) = 3
read(3, "8\22\210')\206Q\200|\317\304\303\6n4{mcs\244\240[\241fh\4\363\340`\244\355\241"..., 100) = 100
write(1, "Read 100 bytes from /dev/random:"..., 33) = 33
write(1, "38 12 88 27 29 86 51 80 7C CF C4"..., 302) = 302
close(3)                                = 0
exit_group(0)                           = ?
+++ exited with 0 +++
```

В данном случае системными вызовами файловой системы (fs/) являются:

* **openat(dirfd, path, flags)** — открыть файл или устройство, вернуть файловый дескриптор.
* **fstat(fd, statbuf)** — получить метаданные файла.
* **close(fd)** — закрыть файловый дескриптор, освободить ресурсы ядра.
* **read(fd, buf, count)** — прочитать до count байт из файла/устройства в память.
* **write(fd, buf, count)** — записать данные из памяти в файл/устройство.
* **access(path, mode)** — проверить доступность файла.
* **pread64(fd, buf, count, offset)** — прочитать данные из файла по смещению.

Вызовами подсистемы управления памятью (mm/) являются:

* **brk(addr)** — увеличить или уменьшить сегмент кучи (используется malloc).
* **mmap(addr, length, prot, flags, fd, offset)** — отобразить файл или анонимную область памяти в адресное пространство процесса.
* **munmap(addr, length)** — убрать отображение памяти (освободить область).
* **mprotect(addr, length, prot)** — изменить права доступа для области памяти.
* **arch_prctl(option, addr)** — архитектурно-специфичная настройка потока.
* **prlimit64(pid, resource, new, old)** — получить или установить лимиты ресурсов.

# Задача №3

Найти системные вызовы в исходниках ядра для вашего ядра.
Версия ядра: 6.8.0-87

Для поиска системных вызово воспользуемся утилитой grep. Для поиска следует вызвать утилиту в следующем виде:
``` grep -w -R "SYSCALL_DEFINE.*СистемныйВызов" ДиректорияПоиска/ ```

Например, для поиска вызова openat в файловой системе команда будет выглядеть так:
``` grep -w -R "SYSCALL_DEFINE.*openat" fs/ ```

### Примеры системных вызовов, прочитанных утилитой grep

| Системный вызов      | Исходный файл в ядре | Определение |
|----------------------|----------------------|-------------|
| openat               | fs/open.c            | SYSCALL_DEFINE4(openat, int dfd, const char __user *filename, int flags, umode_t mode) |
| fstat                | fs/stat.c            | SYSCALL_DEFINE2(fstat, unsigned int, fd, struct __old_kernel_stat __user *, statbuf) |
| access               | fs/open.c            | SYSCALL_DEFINE2(access, const char, __user *, filename, int, mode) |
| mprotect             | mm/mprotect.c        | SYSCALL_DEFINE3(mprotect, unsigned long, start, size_t, len) |
| munmap               | mm/mmap.c            | SYSCALL_DEFINE2(munmap, unsigned long, addr, size_t, len) |
