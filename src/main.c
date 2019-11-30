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
#include "wfi/wfi.h"

int
main(int argc, char **argv) {
    struct wfi_core core;
    const char *filename = argv[1];

    if (argc <= 1) {
        fprintf(stderr, "Usage: %s filename execvp [args...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (wfi_core_init(&core, filename)) {
        fprintf(stderr, "wfi init failed\n");
        return -1;
    }
    puts("wfi: init configured");
    if (wfi_core_attach(&core)) {
        fprintf(stderr, "wfi attach failed\n");
        return -1;
    }
    puts("wfi: core attached");
    if (wfi_wait_for_input(&core)) {
        fprintf(stderr, "wfi attach failed\n");
        return -1;
    }
    puts("wfi: wait for input terminated");
    if (wfi_core_release(&core)) {
        fprintf(stderr, "wfi release failed\n");
        return -1;
    }
    puts("wfi: core released");
    wfi_core_destroy(&core);
    puts("wfi: memory free");
    return 0;
}