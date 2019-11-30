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

int
wfi_parse_json(struct wfi_core *core, const char *buffer) {
    cJSON *json_wfi;
    const cJSON *json_poll = NULL;
    const cJSON *json_gpios = NULL;
    const cJSON *json_gpio = NULL;
    const cJSON *json_poll_timeout = NULL;
    const cJSON *json_poll_maxfd = NULL;
    int index = 0;

    json_wfi = cJSON_Parse(buffer);
    if (json_wfi == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        fprintf(stderr, "wfi json parse: %s\n", error_ptr);
        cJSON_Delete(json_wfi);
        return -1;
    }

    json_poll = cJSON_GetObjectItemCaseSensitive(json_wfi, "poll");
    if (json_poll != NULL) {
        /* core->timeout configuration */
        json_poll_timeout = cJSON_GetObjectItemCaseSensitive(json_poll, "timeout");
        if (cJSON_IsNumber(json_poll_timeout)) {
            printf("Checking poll timeout value \"%d\"\n", json_poll_timeout->valueint);
            core->timeout = json_poll_timeout->valueint;
        } else {
            core->timeout = -1;
        }

        /* core->maxfd configuration */
        json_poll_maxfd = cJSON_GetObjectItemCaseSensitive(json_poll, "maxfd");
        if (cJSON_IsNumber(json_poll_maxfd)) {
            printf("Checking poll maxfd value \"%d\"\n", json_poll_maxfd->valueint);
            if (json_poll_maxfd->valueint <= 0) {
                fprintf(stderr, "wfi json require a valid maxfd must be more than 0\n");
                cJSON_Delete(json_wfi);
                return -1;
            }
            core->maxfd = json_poll_maxfd->valueint;
        } else {
            core->maxfd = 128;
        }
    } else {
        /* if configuration it's not overwrited by the user */
        core->timeout = -1;
        core->maxfd = 128;
    }

    json_gpios = cJSON_GetObjectItemCaseSensitive(json_wfi, "gpios");
    if (json_gpios == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        fprintf(stderr, "wfi json parse: %s\n", error_ptr);
        cJSON_Delete(json_wfi);
        return -1;
    }

    if ((core->pfdds = malloc(sizeof(struct wfi_pfdd) * core->maxfd)) == NULL) {
        fprintf(stderr, "wfi json core malloc: malloc failed\n");
        cJSON_Delete(json_wfi);
        return -1;
    }

    if ((core->pfds = malloc(sizeof(struct pollfd) * core->maxfd)) == NULL) {
        fprintf(stderr, "wfi json core malloc: malloc failed\n");
        cJSON_Delete(json_wfi);
        return -1;
    }

    cJSON_ArrayForEach(json_gpio, json_gpios) {
        cJSON *name = cJSON_GetObjectItemCaseSensitive(json_gpio, "name");
        cJSON *gpio_number = cJSON_GetObjectItemCaseSensitive(json_gpio, "gpio_number");
        cJSON *sh = cJSON_GetObjectItemCaseSensitive(json_gpio, "sh");
        cJSON *gpio_direction = cJSON_GetObjectItemCaseSensitive(json_gpio, "direction");
        cJSON *gpio_edge = cJSON_GetObjectItemCaseSensitive(json_gpio, "edge");

        if (index >= core->maxfd) {
            fprintf(stderr, "wfi json parse: limit of %d gpio raised\n", core->maxfd);
            cJSON_Delete(json_wfi);
            return -1;
        }

        if (cJSON_IsString(name) && (name->valuestring != NULL)) {
            printf("Checking name \"%s\"\n", name->valuestring);
            core->pfdds[index].name = strdup(name->valuestring);
        }

        if (cJSON_IsString(gpio_edge) && (gpio_edge->valuestring != NULL)) {
            printf("Checking edge \"%s\"\n", gpio_edge->valuestring);
            if (strcmp(gpio_edge->valuestring, "none") == 0)
                core->pfdds[index].gpio_edge = GPIO_EDGE_NONE;
            else if (strcmp(gpio_edge->valuestring, "falling") == 0)
                core->pfdds[index].gpio_edge = GPIO_EDGE_FALLING;
            else if (strcmp(gpio_edge->valuestring, "rising") == 0)
                core->pfdds[index].gpio_edge = GPIO_EDGE_RISING;
            else if (strcmp(gpio_edge->valuestring, "both") == 0)
                core->pfdds[index].gpio_edge = GPIO_EDGE_BOTH;
            else {
                fprintf(stderr, "wfi json parse: edge invalid %s\n", gpio_edge->valuestring);
                cJSON_Delete(json_wfi);
                return -1;
            }
        } else {
            core->pfdds[index].gpio_edge = GPIO_EDGE_RISING;
            printf("Default edge rising\n");
        }

        if (cJSON_IsString(gpio_direction) && (gpio_direction->valuestring != NULL)) {
            printf("Checking direction \"%s\"\n", gpio_direction->valuestring);
            if (strcmp(gpio_direction->valuestring, "out") == 0)
                core->pfdds[index].gpio_dir = GPIO_DIR_OUT;
            else if (strcmp(gpio_direction->valuestring, "in") == 0)
                core->pfdds[index].gpio_dir = GPIO_DIR_IN;
            else {
                fprintf(stderr, "wfi json parse: direction invalid %s\n", gpio_direction->valuestring);
                cJSON_Delete(json_wfi);
                return -1;
            }
        } else {
            core->pfdds[index].gpio_dir = GPIO_DIR_IN;
            printf("Default direction in\n");
        }

        if (cJSON_IsNumber(gpio_number)) {
            printf("Checking gpio \"%d\"\n", gpio_number->valueint);
            core->pfdds[index].gpio_number = gpio_number->valueint;
        } else {
            fprintf(stderr, "wfi json parse: no gpio number provided\n");
            cJSON_Delete(json_wfi);
            return -1;
        }

        if (cJSON_IsString(sh) && (sh->valuestring != NULL)) {
            printf("Checking sh \"%s\"\n", sh->valuestring);
            core->pfdds[index].cmd = strdup(sh->valuestring);
        } else {
            fprintf(stderr, "wfi json parse: no sh command provided\n");
            cJSON_Delete(json_wfi);
            return -1;
        }
        index++;
    }
    core->numberfd = index;
    cJSON_Delete(json_wfi);
    return 0;
}