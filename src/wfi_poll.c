/*
 * File: wfi_poll.c
 * Project: src
 * Author: Walter Bonetti <bonettiw@amotus.ca>
 * MIT - 2018 - 2019
 */

#include <stdio.h>
#include <poll.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "wfi/wfi.h"

int
wfi_execute(struct wfi_pfdd *data, int rd_value) {
    (void) rd_value; /* prepare another feature */
    return fork_sh_execute(data->cmd);
}

int
wfi_wait_for_input(struct wfi_core *core) {
    struct pollfd *pfds = core->pfds;
    struct wfi_pfdd *pfdds = core->pfdds;
    uint8_t buf[8];
    int index;
    int ret;

    while (1) {
        ret = poll(pfds, core->numberfd, core->timeout);
        index = 0;
        while (index < core->numberfd)
        {
            lseek(pfds[index].fd, 0, SEEK_SET);
            if (pfds[index].revents & POLLPRI)
            {
                /* process actual fd/gpio event */
                ret = read(pfds[index].fd, buf, sizeof(buf));
                if (ret < 0) {
                    fprintf(stderr, "wfi read: %s\n", strerror(errno));
                    return EXIT_FAILURE;
                }
                /* safety read */
                buf[7] = 0;
                /* execute user command process */
                ret = wfi_execute(&pfdds[index], atoi(buf));
                if (ret != 0) {
                    fprintf(stderr, "wfi execute: %s - code: %d\n", strerror(ret), ret);
                }
            }
            ++index;
        }
    }
}