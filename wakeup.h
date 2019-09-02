/* SPDX-License-Identifier: BSD-3-Clause
 *
 * Goodix FOD Wakeup
 * Tiny service for in-display fingerprint to do tap-to-wake-and-scan
 *
 * Copyright (c) 2019 Nanda Oktavera
 * Released under the terms of 3-clause BSD License
 *
 */

//#define DEBUG

#ifndef GOODIXTS_WAKEUP_H
#define GOODIXTS_WAKEUP_H

#define EVDEV   "/dev/input/event2" // goodix_ts input event path
#define BLDEV   "/sys/class/backlight/panel0-backlight/bl_power"
#define INP_OFF 325 // when area is touched while screen is off
#define DELAY   10000

#ifdef DEBUG
#define dbg printf
#else
#define dbg(fmt,...) do {} while (0)
#endif

int send_input(char *input, uint16_t type, uint16_t code, uint16_t value);
int readfint(char *file);

#define APP_VERSION "1.2"
#endif
