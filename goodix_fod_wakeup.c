/* SPDX-License-Identifier: BSD-3-Clause
 *
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
#include <linux/fb.h>
#include <stdlib.h>

#define EVDEV   "/dev/input/event2" // goodix_ts input event path
#define BLDEV   "/sys/class/backlight/panel0-backlight/bl_power"
#define INP_OFF 325 // when screen is off
#define DELAY   10000

int send_input(char *input, uint16_t type, uint16_t code, uint16_t value)
{
	int fd, ret = 1;
	struct input_event ev;

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

void wakeup_nezuko()
{
	if (readfint(BLDEV) <= FB_BLANK_NORMAL) // screen is already on
		return;

	printf(":: Wake-up the screen\n");

	send_input(EVDEV, EV_KEY, KEY_WAKEUP, 1);
	send_input(EVDEV, EV_SYN, SYN_REPORT, 0);

	send_input(EVDEV, EV_KEY, KEY_WAKEUP, 0);
	send_input(EVDEV, EV_SYN, SYN_REPORT, 0);

	for(;;) {
		usleep(DELAY);

		if (readfint(BLDEV) != FB_BLANK_UNBLANK)
			continue;

		printf(":: Screen on.\n");
		printf(":: Emulating touches\n");

		send_input(EVDEV, EV_KEY, BTN_TOUCH, 1);
		send_input(EVDEV, EV_SYN, SYN_REPORT, 0);

		send_input(EVDEV, EV_ABS, ABS_MT_TRACKING_ID, 2060);
		send_input(EVDEV, EV_SYN, SYN_REPORT, 0);

		send_input(EVDEV, EV_KEY, BTN_TOUCH, 0);
		send_input(EVDEV, EV_SYN, SYN_REPORT, 0);

		printf(":: Done\n");
		return;
	}
}

int main()
{
	int fd, ret = 0;
	struct input_event ev;
	size_t evsize = sizeof(struct input_event);

	printf("\nFODWakeup 1.1\nCopyright 2019, Nanda Oktavera\n");

	printf(":: Reading %s\n", EVDEV);
	fd = open(EVDEV, O_RDONLY | O_NONBLOCK);

	while(read(fd, &ev, evsize)) {
		if (ev.code == INP_OFF && ev.value == 1)
			wakeup_nezuko();

		usleep(DELAY);
	}

	close(fd);
	return ret;
}
