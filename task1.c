#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define BUFFER_SIZE 1024

int main() {
    char src_path[256];
    char dest_path[256];
    int src_fd;
    int dest_fd;
    ssize_t bytes_read, bytes_written, total_bytes = 0;
    char buffer[BUFFER_SIZE];

    printf("Enter source file path: ");
    if (scanf("%255s", src_path) != 1) {
        fprintf(stderr, "Error: failed to read source path\n");
        return 1;
    }

    printf("Enter destination file path: ");
    if (scanf("%255s", dest_path) != 1) {
        fprintf(stderr, "Error: failed to read destination path\n");
        return 1;
    }

    src_fd = open(src_path, O_RDONLY);
    if (src_fd < 0) {
        fprintf(stderr, "Error opening source file: %s\n", strerror(errno));
        return 1;
    }

    dest_fd = open(dest_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dest_fd < 0) {
        fprintf(stderr, "Error opening destination file: %s\n", strerror(errno));
        close(src_fd);
        return 1;
    }

    while ((bytes_read = read(src_fd, buffer, BUFFER_SIZE)) > 0) {
        bytes_written = write(dest_fd, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            fprintf(stderr, "Error writing to destination file: %s\n", strerror(errno));
            close(src_fd);
            close(dest_fd);
            return 1;
        }
        total_bytes += bytes_written;
    }

    if (bytes_read < 0) {
        fprintf(stderr, "Error reading source file: %s\n", strerror(errno));
        close(src_fd);
        close(dest_fd);
        return 1;
    }

    printf("Copied %zd bytes successfully.\n", total_bytes);

    close(src_fd);
    close(dest_fd);
    return 0;
}

