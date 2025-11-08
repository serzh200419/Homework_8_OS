#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main() {
    const char *filename = "sparse.bin";
    int fd;
    const char *start = "START";
    const char *end = "END";
    off_t offset;
    off_t filesize;

    // --- Step 1: Create sparse file ---
    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        fprintf(stderr, "Error creating %s: %s\n", filename, strerror(errno));
        return 1;
    }

    // --- Step 2: Write "START" ---
    if (write(fd, start, strlen(start)) < 0) {
        fprintf(stderr, "Error writing START: %s\n", strerror(errno));
        close(fd);
        return 1;
    }

    // --- Step 3: Seek forward by 1 MiB from current position ---
    offset = lseek(fd, 1024 * 1024, SEEK_CUR);
    if (offset < 0) {
        fprintf(stderr, "Error seeking: %s\n", strerror(errno));
        close(fd);
        return 1;
    }

    // --- Step 4: Write "END" ---
    if (write(fd, end, strlen(end)) < 0) {
        fprintf(stderr, "Error writing END: %s\n", strerror(errno));
        close(fd);
        return 1;
    }

    close(fd);

    // --- Step 5: Reopen read-only and check size ---
    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Error reopening %s: %s\n", filename, strerror(errno));
        return 1;
    }

    filesize = lseek(fd, 0, SEEK_END);
    if (filesize < 0) {
        fprintf(stderr, "Error getting file size: %s\n", strerror(errno));
        close(fd);
        return 1;
    }

    printf("Apparent file size = %ld bytes\n", filesize);

    /*
      This file appears large because lseek() skipped over 1 MiB without writing.
      The skipped region is a hole in the file. It occupies no actual disk blocks.
      Tools like du report only the real disk usage which is small,
      while 'ls -l' or lseek(SEEK_END) show the apparent logical size.
    */

    close(fd);
    return 0;
}

