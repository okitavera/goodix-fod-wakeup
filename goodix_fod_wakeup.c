// SPDX-License-Identifier: BSD-3-Clause
/*
 * Goodix FOD Wakeup
 *
 * Copyright (c) 2019 Nanda Oktavera
 * Released under the terms of 3-clause BSD License
 *
 */
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <stdlib.h>

#define EVDEV   "/dev/input/event2" // goodix_ts input event path
#define INP_OFF 325 // when screen is off
#define INP_ON  358 // when screen is on
#define DELAY   10000

int send_input(uint16_t type, uint16_t code, uint16_t value)
{
	int fd, ret = 0;
	struct input_event ev;

	fd = open(EVDEV, O_RDWR);
	if (fd < 0) {
		printf(":: Failed to access %s\n", EVDEV);
		ret = 1;
		goto out;
	}

	ev.type = type;
	ev.code = code;
	ev.value = value;

	ret = write(fd, &ev, sizeof(struct input_event));

out:
	close(fd);
	return ret;
}

void wakeup_nezuko()
{
	printf(":: Wake-up the screen...\n");
	send_input(EV_KEY, KEY_WAKEUP, 1);
	send_input(EV_SYN, SYN_REPORT, 0);
	usleep(DELAY);
	send_input(EV_KEY, KEY_WAKEUP, 0);
	send_input(EV_SYN, SYN_REPORT, 0);
	printf(":: Screen on.\n");
}


int main()
{
	int fd, ret = 0;
	struct input_event ev;
	size_t evsize = sizeof(struct input_event);

	printf("\nFODWakeup 1.0\n Copyright 2019, Nanda Oktavera\n");

	printf(":: Reading %s\n", EVDEV);
	fd = open(EVDEV, O_RDONLY);
	while(read(fd, &ev, evsize)) {
		if (ev.code == INP_OFF && ev.value == 1)
			wakeup_nezuko();

		usleep(DELAY);
	}

	close(fd);
	return ret;
}

