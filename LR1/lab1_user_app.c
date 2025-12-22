#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <string.h>

#define IOCTL_GET_HIST_LEN _IOR('x', 1, int)
#define IOCTL_GET_HIST_BUF _IOR('x', 2, size_t[20])

#define DEVICE_PATH "/dev/lab1dev"
#define BIN_COUNT   20
#define ITERATIONS  1000

int main(void)
{
    srand(time(NULL));

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

    printf("Generating %d samples with random delays (0-1000 µs)...\n", ITERATIONS);

    for (int val = 0; val < ITERATIONS; ++val) {
        if (write(wr_fd, &val, sizeof(val)) != sizeof(val)) {
            perror("Write failed");
            goto cleanup;
        }

        unsigned int delay_us = rand() % 1001;
        usleep(delay_us);

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

    // Подсчитаем общее количество измерений
    size_t total_samples = 0;
    for (int i = 0; i < hist_len; ++i) {
        total_samples += histogram[i];
    }

    printf("\n=== Histogram of Read Latencies ===\n");
    printf("Bin width: ~50 µs | Total bins: %d | Total samples: %zu\n", hist_len, total_samples);
    for (int i = 0; i < hist_len; ++i) {
        printf("Bin %02d (%4zu-%4zu µs): %zu samples\n",
               i,
               i * 50,
               (i + 1) * 50 - 1,
               histogram[i]);
    }

cleanup:
    close(wr_fd);
    close(rd_fd);
    return EXIT_SUCCESS;
}