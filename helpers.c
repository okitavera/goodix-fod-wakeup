/* SPDX-License-Identifier: BSD-3-Clause
 *
 * Goodix FOD Wakeup
 * Tiny service for in-display fingerprint to do tap-to-wake-and-scan
 *
 * Copyright (c) 2019 Nanda Oktavera
 * Released under the terms of 3-clause BSD License
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include "wakeup.h"

int send_input(char *input, uint16_t type, uint16_t code, uint16_t value)
{
	int fd, ret = 1;
	struct input_event ev = {};

	fd = open(input, O_RDWR | O_NONBLOCK);
	if (fd < 0)
		goto out;

	ev.type = type;
	ev.code = code;
	ev.value = value;

	ret = write(fd, &ev, sizeof(struct input_event));
	if (type == EV_SYN && code == SYN_REPORT)
		usleep(DELAY);

out:
	close(fd);
	return ret;
}

int readfint(char *file)
{
	int fd, ret = -1;
	char buf[8];
	ssize_t len;

	fd = open(file, O_RDONLY);
	if (fd < 0)
		goto out;

	len = read(fd, buf, sizeof(buf) - 1);
	if (len <= 0)
		goto out;

	buf[len - 1] = 0;
	sscanf(buf, "%d", &ret);

out:
	close(fd);
	return ret;
}