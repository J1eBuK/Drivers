#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

// Чтение из /dev/zero
int read_zero() {
    int fd = open("/dev/zero", O_RDONLY);
    if (fd < 0) {
        perror("open /dev/zero");
        return;
    }

    unsigned char buf[16];
    int n = read(fd, buf, sizeof(buf));
    if (n < 0) {
        perror("read /dev/zero");
        close(fd);
        return;
    }

    printf("Read %d bytes from /dev/zero:\n", n);
    for (int i = 0; i < n; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n\n");

    close(fd);
    return 0
}

// Чтение из /dev/random
int read_random() {
    int fd = open("/dev/random", O_RDONLY);
    if (fd < 0) {
        perror("open /dev/random");
        return;
    }

    unsigned char buf[16];
    int n = read(fd, buf, sizeof(buf));
    if (n < 0) {
        perror("read /dev/random");
        close(fd);
        return;
    }

    printf("Read %d bytes from /dev/random:\n", n);
    for (int i = 0; i < n; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n\n");

    close(fd);
    return 0
}

int main() {
    printf("=== Reading /dev/zero ===\n");
    read_zero();

    printf("=== Reading /dev/random ===\n");
    read_random();

    return 0;
}
