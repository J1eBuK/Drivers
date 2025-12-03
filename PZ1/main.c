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

// Чтение из /dev/random
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
