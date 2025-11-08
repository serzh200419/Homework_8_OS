#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main() {
    char path[256];
    int fd;
    off_t size;
    char ch;

    printf("Enter file path: ");
    if (scanf("%255s", path) != 1) {
        fprintf(stderr, "Error: failed to read path.\n");
        return 1;
    }

    fd = open(path, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Error opening file: %s\n", strerror(errno));
        return 1;
    }

    size = lseek(fd, 0, SEEK_END);
    if (size < 0) {
        fprintf(stderr, "Error getting file size: %s\n", strerror(errno));
        close(fd);
        return 1;
    }

    if (size == 0) {
        printf("\n");
        close(fd);
        return 0;
    }

    for (off_t pos = size - 1; pos >= 0; pos--) {
        if (lseek(fd, pos, SEEK_SET) < 0) {
            fprintf(stderr, "Error seeking file: %s\n", strerror(errno));
            close(fd);
            return 1;
        }

        if (read(fd, &ch, 1) != 1) {
            fprintf(stderr, "Error reading file: %s\n", strerror(errno));
            close(fd);
            return 1;
        }

        if (write(STDOUT_FILENO, &ch, 1) != 1) {
            fprintf(stderr, "Error writing output: %s\n", strerror(errno));
            close(fd);
            return 1;
        }
    }

    write(STDOUT_FILENO, "\n", 1);
}
