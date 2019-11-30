/*
 * File: gpio.h
 * Project: wfi
 * Author: Walter Bonetti <bonettiw@amotus.ca>
 * MIT - 2018 - 2019
 */

#include <stdint.h>
#include <poll.h>
#include <stdlib.h>
#include <fcntl.h>

#if !defined(_GPIO_HEADER)
#define _GPIO_HEADER

int wfi_gpio_open_value(int gpio_number, struct pollfd *pfd);
int wfi_gpio_export(int gpio_number, const char *name);
int wfi_gpio_unexport(int gpio_number);
int wfi_gpio_direction(int gpio_number, uint8_t direction);
int wfi_gpio_edge(int gpio_number, uint8_t edge);

#endif // _GPIO_HEADER
