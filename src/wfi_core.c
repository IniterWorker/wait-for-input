/*
 * File: wfi_core.c
 * Project: src
 * Author: Walter Bonetti <bonettiw@amotus.ca>
 * MIT - 2018 - 2019
 */

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "wfi/gpio.h"
#include "wfi/wfi.h"

static int
__core_gpio_attach(struct wfi_core *core, int index)
{
	if (wfi_gpio_export(core->pfdds[index].gpio_number, core->pfdds[index].name) != 0)
	{
		if (!core->pfdds[index].is_al_exp)
		{
			fprintf(stderr, "%s: export failed\n", __func__);
			return -1;
		}
	}
	if (wfi_gpio_direction(core->pfdds[index].gpio_number, core->pfdds[index].gpio_dir) != 0)
	{
		fprintf(stderr, "%s: direction set failed\n", __func__);
		return -1;
	}
	if (wfi_gpio_edge(core->pfdds[index].gpio_number, core->pfdds[index].gpio_edge) != 0)
	{
		fprintf(stderr, "%s: edge set failed\n", __func__);
		return -1;
	}
	return wfi_gpio_open_value(core->pfdds[index].gpio_number, &core->pfds[index]);
}

static int
__core_gpio_release(struct wfi_core *core, int index)
{
	if (wfi_gpio_unexport(core->pfdds[index].gpio_number) < 0)
	{
		fprintf(stderr, "%s: unexport failed\n", __func__);
		return -1;
	}
}

int wfi_core_attach(struct wfi_core *core)
{
	int index = 0;

	while (index < core->numberfd)
	{
		printf("%s: add gpio %d\n", __func__, core->pfdds[index].gpio_number);
		if (__core_gpio_attach(core, index) < 0)
		{
			fprintf(stderr, "%s: rollback attach after failed\n", __func__);
			while (index > 0)
			{
				__core_gpio_release(core, index--);
			}
			return -1;
		}
		index++;
	}
	return 0;
}

int wfi_core_release(struct wfi_core *core)
{
	int index = 0;

	while (index < core->numberfd)
	{
		__core_gpio_release(core, index++);
	}
	return 0;
}

void wfi_core_destroy(struct wfi_core *core)
{
	int index = 0;

	/* free internal allocated data */
	while (index < core->numberfd)
	{
		if (core->pfdds[index].name)
		{
			free(core->pfdds[index].name);
		}
		if (core->pfdds[index].cmd)
		{
			free(core->pfdds[index].cmd);
		}
	}

	/* free allocated data */
	if (core->pfdds)
		free(core->pfdds);
	if (core->pfdds)
		free(core->pfds);

	/* clean */
	bzero(core, sizeof(struct wfi_core));
}