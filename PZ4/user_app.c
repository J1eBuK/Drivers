#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <errno.h>

#define DEVICE "/dev/pz4"
#define MODULE_MAGIC 'p'
#define MODULE_IOC_CLEAR  _IO(MODULE_MAGIC, 0)
#define MODULE_IOC_IS_EMPTY    _IOR(MODULE_MAGIC, 1, int)

static void usage(const char *p) {
    printf("Usage: %s write <text> | read <n> | clear | isempty\n", p);
}

int main(int argc, char **argv)
{
    if (argc < 2) { usage(argv[0]); return 1; }

    int fd = open(DEVICE, O_RDWR);
    if (fd < 0) { perror("open"); return 1; }

    if (strcmp(argv[1], "write") == 0) {
        if (argc < 3) { printf("write needs text\n"); close(fd); return 1; }
        const char *s = argv[2];
        ssize_t w = write(fd, s, strlen(s));
        if (w < 0) perror("write"); else printf("wrote %zd bytes\n", w);
    }
    else if (strcmp(argv[1], "read") == 0) {
        int n = (argc >= 3) ? atoi(argv[2]) : 64;
        char *buf = malloc(n+1);
        if (!buf) { close(fd); return 1; }
        ssize_t r = read(fd, buf, n);
        if (r < 0) perror("read"); else { buf[r] = 0; printf("read %zd bytes: '%s'\n", r, buf); }
        free(buf);
    }
    else if (strcmp(argv[1], "clear") == 0) {
        if (ioctl(fd, MODULE_IOC_CLEAR) < 0) perror("ioctl clear"); else printf("cleared\n");
    }
    else if (strcmp(argv[1], "isempty") == 0) {
        int len = 0;
        if (ioctl(fd, MODULE_IOC_IS_EMPTY, &len) < 0) perror("ioctl len"); else printf("len = %d\n", len);
    }
    else { usage(argv[0]); close(fd); return 1; }

    close(fd);
    return 0;
}
