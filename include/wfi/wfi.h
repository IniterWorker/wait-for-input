/*
 * File: wfi.h
 * Project: include
 * Author: Walter Bonetti <bonettiw@amotus.ca>
 * MIT - 2018 - 2019
 */

#ifndef _WFI_HEADER
#define _WFI_HEADER

#include <poll.h>
#include <stdint.h>
#include <time.h>

#define GPIO_EDGE_NONE 0
#define GPIO_EDGE_RISING 1
#define GPIO_EDGE_FALLING 2
#define GPIO_EDGE_BOTH 3

#define GPIO_DIR_IN 0
#define GPIO_DIR_OUT 1

struct wfi_pfdd
{
	char *name; 				/* possible name */
	char *cmd;					/* command line execve */
	struct timespec last_time;  /* reference poll debounce */
	uint32_t gpio_number;		/* gpio number */
	uint32_t debounce_ms;		/* debounce ms */
	uint8_t gpio_edge;			/* edge */
	uint8_t gpio_dir;			/* direction */
	uint8_t is_endfixed;		/* TODO: improve pass argument to the user sh env */
	uint8_t is_al_exp;			/* allow use already exported gpio */
};

struct wfi_core
{
	int timeout;			/* poll timeout */
	nfds_t maxfd;			/* poll maxfd */
	nfds_t numberfd;		/* current item in pollfd array */
	struct pollfd *pfds;	/* pollfd array */
	struct wfi_pfdd *pfdds; /* pollfd data array */
};

int fork_sh_execute(const char *command);
int wfi_check_access(const char *path);
int wfi_wait_for_input(struct wfi_core *core);
int wfi_parse_json(struct wfi_core *core, const char *buffer);

/* core */
int wfi_core_init(struct wfi_core *core, const char *filename);
int wfi_core_attach(struct wfi_core *core);
int wfi_core_release(struct wfi_core *core);
void wfi_core_destroy(struct wfi_core *core);

#endif // _WFI_HEADER
