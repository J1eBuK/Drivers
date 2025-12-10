#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#define DEVICE "/dev/prikoldev"

#define prikol_MAGIC 'k'
#define prikol_IOC_CLEAR _IO(prikol_MAGIC, 0)
#define prikol_IOC_IS_EMPTY _IOR(prikol_MAGIC, 1, int)

int main(int argc, char *argv[])
{
    int fd = open(DEVICE, O_RDWR);
    if (fd < 0) { perror("open"); return 1; }

    if (argc >= 2 && strcmp(argv[1], "write") == 0) {
        const char *msg = argc >= 3 ? argv[2] : "Tyt 6.bl.Jl Makc!";
        ssize_t w = write(fd, msg, strlen(msg));
        if (w < 0) { perror("write"); close(fd); return 1; }
        printf("Wrote %zd bytes to device: '%s'\n", w, msg);
    }
    else if (argc >= 2 && strcmp(argv[1], "read") == 0) {
        char buf[512];
        ssize_t r = read(fd, buf, sizeof(buf)-1);
        if (r < 0) { perror("read"); close(fd); return 1; }
        buf[r] = '\0';
        printf("Read %zd bytes: '%s'\n", r, buf);
    }
    else if (argc >= 2 && strcmp(argv[1], "clear") == 0) {
        if (ioctl(fd, prikol_IOC_CLEAR) < 0) { perror("ioctl clear"); close(fd); return 1; }
        printf("Buffer cleared (ioctl)\n");
    }
    else if (argc >= 2 && strcmp(argv[1], "isempty") == 0) {
        int empty = -1;
        if (ioctl(fd, prikol_IOC_IS_EMPTY, &empty) < 0) { perror("ioctl isempty"); close(fd); return 1; }
        printf("Buffer empty? %s\n", empty ? "YES" : "NO");
    }
    else {
        printf("Usage: %s [write <msg> | read | clear | isempty]\n", argv[0]);
    }

    close(fd);
    return 0;
}
