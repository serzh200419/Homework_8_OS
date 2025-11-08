#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#define MAX_INPUT 512

int main() {
    const char *filename = "log.txt";
    int fd;
    char input[MAX_INPUT];
    ssize_t nread;

    fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) {
        fprintf(stderr, "Error opening %s: %s\n", filename, strerror(errno));
        return 1;
    }

    printf("Enter a log message: ");
    fflush(stdout); // Ensure prompt appears before input
    nread = read(STDIN_FILENO, input, MAX_INPUT - 1);

    if (nread < 0) {
        fprintf(stderr, "Error reading input: %s\n", strerror(errno));
        close(fd);
        return 1;
    }

    if (nread > 0 && input[nread - 1] == '\n') {
        input[nread - 1] = '\0';
        nread--;
    } else {
        input[nread] = '\0';
    }

    char entry[MAX_INPUT + 64];
    pid_t pid = getpid();
    snprintf(entry, sizeof(entry), "PID=%d: %s\n", pid, input);

    ssize_t bytes_written = write(fd, entry, strlen(entry));
    if (bytes_written < 0) {
        fprintf(stderr, "Error writing to file: %s\n", strerror(errno));
        close(fd);
        return 1;
    }

    off_t offset = lseek(fd, 0, SEEK_CUR);
    if (offset < 0) {
        fprintf(stderr, "Error getting file offset: %s\n", strerror(errno));
        close(fd);
        return 1;
    }

    printf("Appended %zd bytes. Current file offset = %ld\n", bytes_written, offset);

    /*
      Even though the file was opened with O_APPEND,
      every write() automatically appends data to the end of the file.
      However, lseek(fd, 0, SEEK_CUR) still tracks the logical offset
      after the last write. So it grows with each append.
      The kernel maintains the offset position internally even though
      O_APPEND causes the write pointer to move to EOF for every write.
    */

    close(fd);
    return 0;
}

