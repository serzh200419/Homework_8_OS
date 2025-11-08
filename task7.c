#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define BUF_SIZE 1024

int main() {
    char file1_path[256];
    char file2_path[256];
    int fd1;
    int fd2;
    unsigned char buf1[BUF_SIZE], buf2[BUF_SIZE];
    ssize_t n1;
    ssize_t n2;
    off_t byte_index = 0;

    printf("Enter first file path: ");
    if (scanf("%255s", file1_path) != 1) {
        fprintf(stderr, "Error reading first path\n");
        return 1;
    }

    printf("Enter second file path: ");
    if (scanf("%255s", file2_path) != 1) {
        fprintf(stderr, "Error reading second path\n");
        return 1;
    }

    fd1 = open(file1_path, O_RDONLY);
    if (fd1 < 0) {
        fprintf(stderr, "Error opening %s: %s\n", file1_path, strerror(errno));
        return 1;
    }

    fd2 = open(file2_path, O_RDONLY);
    if (fd2 < 0) {
        fprintf(stderr, "Error opening %s: %s\n", file2_path, strerror(errno));
        close(fd1);
        return 1;
    }

    while (1) {
        n1 = read(fd1, buf1, BUF_SIZE);
        if (n1 < 0) { perror("read fd1"); close(fd1); close(fd2); return 1; }

        n2 = read(fd2, buf2, BUF_SIZE);
        if (n2 < 0) { perror("read fd2"); close(fd1); close(fd2); return 1; }

        if (n1 == 0 && n2 == 0) break;

        ssize_t min_n = (n1 < n2) ? n1 : n2;
        for (ssize_t i = 0; i < min_n; i++) {
            if (buf1[i] != buf2[i]) {
                printf("Files differ at byte %ld\n", byte_index + i);
                close(fd1);
                close(fd2);
                return 1;
            }
        }

        byte_index += min_n;

        if (n1 != n2) {
            printf("Files differ at byte %ld\n", byte_index);
            close(fd1);
            close(fd2);
            return 1;
        }
    }

    printf("Files are identical\n");

    close(fd1);
    close(fd2);
    return 0;
}

