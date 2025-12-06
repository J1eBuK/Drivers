// user_app.c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main(void)
{
    const char *dev = "/dev/prikoldev";
    int fd = open(dev, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    char buf[128];
    ssize_t n = read(fd, buf, sizeof(buf)-1);
    if (n < 0) {
        perror("read");
        close(fd);
        return 1;
    }
    buf[n] = '\0';
    printf("Read from device: %s", buf);

    close(fd);
    return 0;
}
