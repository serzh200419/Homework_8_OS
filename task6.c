#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define BUF_SIZE 1024

int main() {
    const char *filename = "numbers.txt";
    int fd;
    char buffer[BUF_SIZE];
    ssize_t nread;
    off_t offset = 0;
    int line = 1;

    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        fprintf(stderr, "Error creating %s: %s\n", filename, strerror(errno));
        return 1;
    }

    for (int i = 1; i <= 10; i++) {
        char linebuf[16];
        int len = snprintf(linebuf, sizeof(linebuf), "%d\n", i);
        if (write(fd, linebuf, len) != len) {
            fprintf(stderr, "Error writing number %d: %s\n", i, strerror(errno));
            close(fd);
            return 1;
        }
    }
    close(fd);

    fd = open(filename, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "Error reopening %s: %s\n", filename, strerror(errno));
        return 1;
    }

    while ((nread = read(fd, buffer, sizeof(buffer))) > 0) {
        for (ssize_t i = 0; i < nread; i++) {
            if (buffer[i] == '\n') {
                line++;
                if (line == 4) {
                    offset += i + 1;
                    goto found_offset;
                }
            }
        }
        offset += nread;
    }

found_offset:
    if (line < 4) {
        fprintf(stderr, "Error: fewer than 4 lines in file.\n");
        close(fd);
        return 1;
    }

    if (lseek(fd, offset, SEEK_SET) < 0) {
        fprintf(stderr, "Error seeking: %s\n", strerror(errno));
        close(fd);
        return 1;
    }

    char remainder[BUF_SIZE * 2];
    ssize_t remainder_len = read(fd, remainder, sizeof(remainder));
    if (remainder_len < 0) {
        fprintf(stderr, "Error reading remainder: %s\n", strerror(errno));
        close(fd);
        return 1;
    }

    if (lseek(fd, offset, SEEK_SET) < 0) {
        fprintf(stderr, "Error seeking back: %s\n", strerror(errno));
        close(fd);
        return 1;
    }

    const char *replacement = "100\n";
    ssize_t rep_len = strlen(replacement);

    if (write(fd, replacement, rep_len) != rep_len) {
        fprintf(stderr, "Error writing replacement: %s\n", strerror(errno));
        close(fd);
        return 1;
    }

    if (write(fd, remainder, remainder_len) != remainder_len) {
        fprintf(stderr, "Error writing remainder: %s\n", strerror(errno));
        close(fd);
        return 1;
    }

    off_t new_size = offset + rep_len + remainder_len;
    if (ftruncate(fd, new_size) < 0) {
        fprintf(stderr, "Error truncating file: %s\n", strerror(errno));
        close(fd);
        return 1;
    }

    lseek(fd, 0, SEEK_SET);
    printf("Final content of %s:\n", filename);
    while ((nread = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[nread] = '\0';
        printf("%s", buffer);
    }
    printf("\n");

    close(fd);
    return 0;
}

