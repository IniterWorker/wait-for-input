/*
 * File: wfi_config.c
 * Project: src
 * Author: Walter Bonetti <bonettiw@amotus.ca>
 * MIT - 2018 - 2019
 */

#include <cjson/cJSON.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "wfi/wfi.h"
#include "wfi/gpio.h"

static int
wfi_parse_json_poll(struct wfi_core *core, cJSON *json_wfi)
{
	const cJSON *json_poll = NULL;
	const cJSON *json_poll_timeout = NULL;
	const cJSON *json_poll_maxfd = NULL;

	json_poll = cJSON_GetObjectItemCaseSensitive(json_wfi, "poll");
	if (json_poll != NULL)
	{
		/* core->timeouclock_gettimet configuration */
		json_poll_timeout = cJSON_GetObjectItemCaseSensitive(json_poll, "timeout");
		if (cJSON_IsNumber(json_poll_timeout))
		{
			printf("Checking poll timeout value \"%d\"\n", json_poll_timeout->valueint);
			core->timeout = json_poll_timeout->valueint;
		}
		else
		{
			core->timeout = -1;
			printf("Default timeout: %d\n", core->timeout);
		}

		/* core->maxfd configuration */
		json_poll_maxfd = cJSON_GetObjectItemCaseSensitive(json_poll, "maxfd");
		if (cJSON_IsNumber(json_poll_maxfd))
		{
			printf("Checking poll maxfd value \"%d\"\n", json_poll_maxfd->valueint);
			if (json_poll_maxfd->valueint <= 0)
			{
				fprintf(stderr, "wfi json require a valid maxfd must be more than 0\n");
				return -1;
			}
			core->maxfd = json_poll_maxfd->valueint;
		}
		else
		{
			core->maxfd = 128;
		}
	}
	else
	{
		/* if configuration it's not overwrited by the user */
		core->timeout = -1;
		core->maxfd = 128;
	}
	return 0;
}

int wfi_parse_json_gpios(struct wfi_core *core, cJSON *json_wfi)
{
	const cJSON *json_gpios = NULL;
	const cJSON *json_gpio = NULL;
	int index;

	index = 0;
	json_gpios = cJSON_GetObjectItemCaseSensitive(json_wfi, "gpios");
	if (json_gpios == NULL)
	{
		const char *error_ptr = cJSON_GetErrorPtr();
		fprintf(stderr, "wfi json parse: %s\n", error_ptr);
		return -1;
	}

	cJSON_ArrayForEach(json_gpio, json_gpios)
	{
		cJSON *name = cJSON_GetObjectItemCaseSensitive(json_gpio, "name");
		cJSON *gpio_number = cJSON_GetObjectItemCaseSensitive(json_gpio, "gpio_number");
		cJSON *sh = cJSON_GetObjectItemCaseSensitive(json_gpio, "sh");
		cJSON *gpio_direction = cJSON_GetObjectItemCaseSensitive(json_gpio, "direction");
		cJSON *gpio_edge = cJSON_GetObjectItemCaseSensitive(json_gpio, "edge");
		cJSON *gpio_debounce = cJSON_GetObjectItemCaseSensitive(json_gpio, "debounce");
		cJSON *gpio_al_exp = cJSON_GetObjectItemCaseSensitive(json_gpio, "allow_already_exported");

		if (index >= core->maxfd)
		{
			fprintf(stderr, "wfi json parse: limit of %ld gpio raised\n", core->maxfd);
			return -1;
		}

		if (cJSON_IsString(name) && (name->valuestring != NULL))
		{
			printf("Checking name \"%s\"\n", name->valuestring);
			core->pfdds[index].name = strdup(name->valuestring);
		}

		if (cJSON_IsString(gpio_edge) && (gpio_edge->valuestring != NULL))
		{
			printf("Checking edge \"%s\"\n", gpio_edge->valuestring);
			if (strcmp(gpio_edge->valuestring, "none") == 0)
				core->pfdds[index].gpio_edge = GPIO_EDGE_NONE;
			else if (strcmp(gpio_edge->valuestring, "falling") == 0)
				core->pfdds[index].gpio_edge = GPIO_EDGE_FALLING;
			else if (strcmp(gpio_edge->valuestring, "rising") == 0)
				core->pfdds[index].gpio_edge = GPIO_EDGE_RISING;
			else if (strcmp(gpio_edge->valuestring, "both") == 0)
				core->pfdds[index].gpio_edge = GPIO_EDGE_BOTH;
			else
			{
				fprintf(stderr, "wfi json parse: edge invalid %s\n", gpio_edge->valuestring);
				return -1;
			}
		}
		else
		{
			core->pfdds[index].gpio_edge = GPIO_EDGE_RISING;
			printf("Default edge rising\n");
		}

		if (cJSON_IsString(gpio_direction) && (gpio_direction->valuestring != NULL))
		{
			printf("Checking direction \"%s\"\n", gpio_direction->valuestring);
			if (strcmp(gpio_direction->valuestring, "out") == 0)
				core->pfdds[index].gpio_dir = GPIO_DIR_OUT;
			else if (strcmp(gpio_direction->valuestring, "in") == 0)
				core->pfdds[index].gpio_dir = GPIO_DIR_IN;
			else
			{
				fprintf(stderr, "wfi json parse: direction invalid %s\n", gpio_direction->valuestring);
				return -1;
			}
		}
		else
		{
			core->pfdds[index].gpio_dir = GPIO_DIR_IN;
			printf("Default direction in\n");
		}

		if (cJSON_IsNumber(gpio_number))
		{
			printf("Checking gpio \"%d\"\n", gpio_number->valueint);
			core->pfdds[index].gpio_number = gpio_number->valueint;
		}
		else
		{
			fprintf(stderr, "wfi json parse: no gpio number provided\n");
			return -1;
		}

		if (cJSON_IsBool(gpio_al_exp))
		{
			printf("Checking gpio debounce \"%d\" ms\n", gpio_al_exp->valueint);
			core->pfdds[index].is_al_exp = gpio_al_exp->valueint;
		}
		else
		{
			core->pfdds[index].is_al_exp = 1; // ms
			printf("Default gpio allow_already_exported \"%d\"\n", core->pfdds[index].is_al_exp);
		}

		if (cJSON_IsNumber(gpio_debounce))
		{
			printf("Checking gpio debounce \"%d\" ms\n", gpio_debounce->valueint);
			core->pfdds[index].debounce_ms = gpio_debounce->valueint;
		}
		else
		{
			core->pfdds[index].debounce_ms = 200; // ms
			printf("Default gpio debounce \"%d\" ms\n", core->pfdds[index].debounce_ms);
		}

		if (cJSON_IsString(sh) && (sh->valuestring != NULL))
		{
			printf("Checking sh \"%s\"\n", sh->valuestring);
			core->pfdds[index].cmd = strdup(sh->valuestring);
		}
		else
		{
			fprintf(stderr, "wfi json parse: no sh command provided\n");
			return -1;
		}
		index++;
	}
	core->numberfd = index;
	return 0;
}

int wfi_parse_json(struct wfi_core *core, const char *buffer)
{
	cJSON *json_wfi;

	json_wfi = cJSON_Parse(buffer);
	if (json_wfi == NULL)
	{
		const char *error_ptr = cJSON_GetErrorPtr();
		fprintf(stderr, "wfi json parse: %s\n", error_ptr);
		cJSON_Delete(json_wfi);
		return -1;
	}

	if (wfi_parse_json_poll(core, json_wfi) < 0)
	{
		fprintf(stderr, "%s: wfi_parse_json_poll failed\n", __func__);
		cJSON_Delete(json_wfi);
		return -1;
	}

	if ((core->pfdds = calloc(core->maxfd, sizeof(struct wfi_pfdd))) == NULL)
	{
		fprintf(stderr, "%s:pfdds calloc failed\n", __func__);
		cJSON_Delete(json_wfi);
		return -1;
	}

	if ((core->pfds = calloc(core->maxfd, sizeof(struct pollfd))) == NULL)
	{
		fprintf(stderr, "%s:pfds calloc failed\n", __func__);
		cJSON_Delete(json_wfi);
		return -1;
	}

	if (wfi_parse_json_gpios(core, json_wfi) < 0)
	{
		fprintf(stderr, "%s:parse gpios failed\n", __func__);
		cJSON_Delete(json_wfi);
		return -1;
	}

	cJSON_Delete(json_wfi);
	return 0;
}