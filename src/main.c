#include <stdio.h>
#include <stdint.h>
#include <poll.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int
fork_execute(char **argv) {
    pid_t pid;
    int status;

    if ((pid = fork()) < 0) {
        fprintf(stderr, "Error fork: %s\n", strerror(errno));
        return EXIT_FAILURE;
    } else if (pid == 0) {
        if (execvp(*argv, argv) < 0) {
            fprintf(stderr, "Error exec: %s\n", strerror(errno));
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    } else {
        while (wait(&status) != pid);
        return EXIT_SUCCESS;
    }
}

int
main(int argc, char **argv) {
    struct pollfd pfd;
    uint8_t buf[8];
    int fd;
    int ret;

    if (argc <= 1) {
        fprintf(stderr, "Usage: %s filename execvp [args...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    if ((fd = open(argv[1], O_RDONLY)) < 0) {
        fprintf(stderr, "Error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    if (access(argv[2], F_OK) < 0) {
        fprintf(stderr, "Error %s: %s\n", argv[2], strerror(errno));
        return EXIT_FAILURE;
    }

    pfd.fd = fd;
    pfd.events = POLLPRI | POLLERR;

    lseek(fd, 0, SEEK_SET);
    read(fd, buf, sizeof(buf));

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (1) {
        poll(&pfd, 1, -1);
        lseek(fd, 0, SEEK_SET);

        ret = read(fd, buf, sizeof(buf));
        if (ret < 0) {
            fprintf(stderr, "Error: %s\n", strerror(errno));
            return EXIT_FAILURE;
        }

        if (fork_execute(&argv[2]) != EXIT_SUCCESS)
            return EXIT_FAILURE;
    }
#pragma clang diagnostic pop
    return EXIT_SUCCESS;
}