/*
 * File: wfi_init.c
 * Project: src
 * Author: Walter Bonetti <bonettiw@amotus.ca>
 * MIT - 2018 - 2019
 */

#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "wfi/wfi.h"

static char *
wfi_read_whole_file(const char *filename)
{
	FILE *f;
	long fsize;
	char *buffer;

	f = fopen(filename, "rb");
	fseek(f, 0, SEEK_END);
	fsize = ftell(f);
	fseek(f, 0, SEEK_SET);
	buffer = malloc(fsize + 1);
	fread(buffer, 1, fsize, f);
	fclose(f);
	buffer[fsize] = 0;
	return buffer;
}

int wfi_core_init(struct wfi_core *core, const char *filename)
{
	char *buffer;

	bzero(core, sizeof(struct wfi_core));
	buffer = wfi_read_whole_file(filename);
	return wfi_parse_json(core, buffer);
}