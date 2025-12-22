#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define IOCTL_GET_HIST_LEN _IOR('x', 1, int)
#define IOCTL_GET_HIST_BUF _IOR('x', 2, size_t[20])
#define IOCTL_RESET_HIST   _IO('x', 3)

#define DEVICE_PATH "/dev/lab1dev"
#define BIN_COUNT   20
#define ITERATIONS  1000

int main(void)
{
    int rd_fd = open(DEVICE_PATH, O_RDONLY);
    if (rd_fd < 0) {
        perror("Failed to open device for reading");
        return EXIT_FAILURE;
    }

    int wr_fd = open(DEVICE_PATH, O_WRONLY);
    if (wr_fd < 0) {
        perror("Failed to open device for writing");
        close(rd_fd);
        return EXIT_FAILURE;
    }


    if (ioctl(rd_fd, IOCTL_RESET_HIST, 0) < 0) {
        perror("IOCTL_RESET_HIST failed");
        goto cleanup;
    }

    for (int val = 0; val < ITERATIONS; ++val) {
        if (write(wr_fd, &val, sizeof(val)) != sizeof(val)) {
            perror("Write failed");
            goto cleanup;
        }

        usleep(100);

        int dummy;
        if (read(rd_fd, &dummy, sizeof(dummy)) != sizeof(dummy)) {
            perror("Read failed");
            goto cleanup;
        }
    }

    int hist_len = 0;
    if (ioctl(rd_fd, IOCTL_GET_HIST_LEN, &hist_len) < 0) {
        perror("IOCTL_GET_HIST_LEN failed");
        goto cleanup;
    }

    if (hist_len <= 0 || hist_len > BIN_COUNT) {
        fprintf(stderr, "Invalid histogram length: %d\n", hist_len);
        goto cleanup;
    }

    size_t histogram[BIN_COUNT] = {0};
    if (ioctl(rd_fd, IOCTL_GET_HIST_BUF, histogram) < 0) {
        perror("IOCTL_GET_HIST_BUF failed");
        goto cleanup;
    }

    printf("Histogram of read latencies (bins of ~50 µs):\n");
    for (int i = 0; i < hist_len; ++i) {
        printf("Bin %02d (%3lu-%3lu µs): %zu samples\n",
               i, i * 50UL, (i + 1) * 50UL - 1, histogram[i]);
    }

cleanup:
    close(wr_fd);
    close(rd_fd);
    return EXIT_SUCCESS;
}