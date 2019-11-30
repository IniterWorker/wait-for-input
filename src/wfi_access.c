/*
 * File: wfi_access.ca
 * Project: src
 * Author: Walter Bonetti <bonettiw@amotus.ca>
 * MIT - 2018 - 2019
 */

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include "wfi/wfi.h"

int wfi_check_access(const char *path) {
    const int ret = access(path, F_OK) < 0;
     if (ret) {
        fprintf(stderr, "Error %s: %s\n", path, strerror(errno));
        return ret;
    }
    return ret;
}