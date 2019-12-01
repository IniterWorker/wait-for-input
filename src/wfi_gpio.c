/*
 * File: gpio.c
 * Project: src
 * Author: Walter Bonetti <bonettiw@amotus.ca>
 * MIT - 2018 - 2019
 */

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "wfi/wfi.h"
#include "wfi/gpio.h"

int wfi_gpio_export(int gpio_number, const char *name)
{
	char buffer[512];

	bzero(buffer, sizeof(buffer));
	sprintf(buffer, "echo %d > /sys/class/gpio/export", gpio_number);
	(void)name;
	return fork_sh_execute(buffer);
}

int wfi_gpio_unexport(int gpio_number)
{
	char buffer[512];

	bzero(buffer, sizeof(buffer));
	sprintf(buffer, "echo %d > /sys/class/gpio/unexport", gpio_number);
	return fork_sh_execute(buffer);
}

int wfi_gpio_direction(int gpio_number, uint8_t direction)
{
	char buffer[512];

	bzero(buffer, sizeof(buffer));
	if (direction == GPIO_DIR_IN)
		sprintf(buffer, "echo %s > /sys/class/gpio/gpio%d/direction", "in", gpio_number);
	if (direction == GPIO_DIR_OUT)
		sprintf(buffer, "echo %s > /sys/class/gpio/gpio%d/direction", "out", gpio_number);
	return fork_sh_execute(buffer);
}

int wfi_gpio_edge(int gpio_number, uint8_t edge)
{
	char buffer[512];

	bzero(buffer, sizeof(buffer));
	if (edge == GPIO_EDGE_NONE)
		sprintf(buffer, "echo %s > /sys/class/gpio/gpio%d/edge", "none", gpio_number);
	if (edge == GPIO_EDGE_RISING)
		sprintf(buffer, "echo %s > /sys/class/gpio/gpio%d/edge", "rising", gpio_number);
	if (edge == GPIO_EDGE_FALLING)
		sprintf(buffer, "echo %s > /sys/class/gpio/gpio%d/edge", "falling", gpio_number);
	if (edge == GPIO_EDGE_BOTH)
		sprintf(buffer, "echo %s > /sys/class/gpio/gpio%d/edge", "both", gpio_number);
	return fork_sh_execute(buffer);
}

/**
 * @brief Gpio open an df and fill the pollfd struct
 * 
 * @param pfd 
 * @param path 
 * @return int 0 if no error else -1 open failed
 */
static int
_wfi_open_gpio_in_pfd(struct pollfd *pfd, const char *path)
{
	uint8_t buf[8];

	pfd->events = POLLPRI | POLLERR;
	if ((pfd->fd = open(path, O_RDONLY)) < 0)
	{
		fprintf(stderr, "wfi_open gpio in pfd: %s\n", strerror(errno));
		fprintf(stderr, "wfi_open failed at path: %s\n", path);
		return pfd->fd;
	}

	if (lseek(pfd->fd, 0, SEEK_SET) < 0)
	{
		fprintf(stderr, "wfi lseek: %s\n", strerror(errno));
		return pfd->fd;
	}

	if (read(pfd->fd, buf, sizeof(buf)) < 0)
	{
		fprintf(stderr, "read lseek: %s\n", strerror(errno));
		return pfd->fd;
	}
	return pfd->fd;
}

int wfi_gpio_open_value(int gpio_number, struct pollfd *pfd)
{
	char buffer[512];

	bzero(buffer, sizeof(buffer));
	sprintf(buffer, "/sys/class/gpio/gpio%d/value", gpio_number);
	return _wfi_open_gpio_in_pfd(pfd, buffer);
}