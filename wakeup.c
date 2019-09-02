/* SPDX-License-Identifier: BSD-3-Clause
 *
 * Goodix FOD Wakeup
 * Tiny service for in-display fingerprint to do tap-to-wake-and-scan
 *
 * Copyright (c) 2019 Nanda Oktavera
 * Released under the terms of 3-clause BSD License
 *
 */

#include <fcntl.h>
#include <unistd.h>
#include <linux/fb.h>
#include <linux/input.h>
#include "wakeup.h"

static void wakeup_nezuko()
{
	if (readfint(BLDEV) <= FB_BLANK_NORMAL) // screen is already on
		return;

	dbg(":: Wake-up the screen\n");

	send_input(EVDEV, EV_KEY, KEY_WAKEUP, 1);
	send_input(EVDEV, EV_SYN, SYN_REPORT, 0);

	send_input(EVDEV, EV_KEY, KEY_WAKEUP, 0);
	send_input(EVDEV, EV_SYN, SYN_REPORT, 0);

	for(;;) {
		if (readfint(BLDEV) != FB_BLANK_UNBLANK)
			continue;

		dbg(":: Screen on.\n");
		dbg(":: Emulating touches\n");

		send_input(EVDEV, EV_KEY, BTN_TOUCH, 1);
		send_input(EVDEV, EV_SYN, SYN_REPORT, 0);

		send_input(EVDEV, EV_ABS, ABS_MT_TRACKING_ID, 2060);
		send_input(EVDEV, EV_SYN, SYN_REPORT, 0);

		send_input(EVDEV, EV_KEY, BTN_TOUCH, 0);
		send_input(EVDEV, EV_SYN, SYN_REPORT, 0);

		dbg(":: Done\n");
		return;
	}
}

int main()
{
	int fd, ret = 0;
	struct input_event ev;
	size_t evsize = sizeof(struct input_event);

	dbg("FODWakeup %s\n", APP_VERSION);
	dbg("Tiny service for in-display fingerprint to do tap-to-wake-and-scan\n");
	dbg("Copyright 2019, Nanda Oktavera\n");

	dbg(":: Reading %s\n", EVDEV);
	fd = open(EVDEV, O_RDONLY | O_NONBLOCK);

	while(read(fd, &ev, evsize)) {
		if (ev.code == INP_OFF && ev.value == 1)
			wakeup_nezuko();

		usleep(DELAY);
	}

	close(fd);
	return ret;
}
