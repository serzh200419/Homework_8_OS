#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main() {
    const char *filename = "data.txt";
    const char *text = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int fd;
    ssize_t bytes_written;
    off_t size;
    char buffer[64];

    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        fprintf(stderr, "Error opening file for writing: %s\n", strerror(errno));
        return 1;
    }

    bytes_written = write(fd, text, strlen(text));
    if (bytes_written < 0) {
        fprintf(stderr, "Error writing to file: %s\n", strerror(errno));
        close(fd);
        return 1;
    }

    close(fd);
    printf("Step 1: Wrote %zd bytes to %s\n", bytes_written, filename);

    fd = open(filename, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "Error reopening file: %s\n", strerror(errno));
        return 1;
    }

    size = lseek(fd, 0, SEEK_END);
    if (size < 0) {
        fprintf(stderr, "Error getting file size: %s\n", strerror(errno));
        close(fd);
        return 1;
    }
    printf("Step 3: Current file size = %ld bytes\n", size);

    if (ftruncate(fd, 10) < 0) {
        fprintf(stderr, "Error truncating file: %s\n", strerror(errno));
        close(fd);
        return 1;
    }

    size = lseek(fd, 0, SEEK_END);
    printf("Step 5: New file size after truncation = %ld bytes\n", size);

    lseek(fd, 0, SEEK_SET);
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        fprintf(stderr, "Error reading file: %s\n", strerror(errno));
        close(fd);
        return 1;
    }
    buffer[bytes_read] = '\0';
    printf("Step 6: Remaining content = \"%s\"\n", buffer);

    close(fd);
    return 0;
}

