/*
 * Copyright © 2011, 2012 Intel Corporation
 * Copyright © 2013 Jonas Ådahl
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the copyright holders not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  The copyright holders make
 * no representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef EVDEV_H
#define EVDEV_H

#include "config.h"

#include <linux/input.h>
#include <libevdev/libevdev.h>

#include "libinput-private.h"

enum evdev_event_type {
	EVDEV_NONE,
	EVDEV_ABSOLUTE_TOUCH_DOWN,
	EVDEV_ABSOLUTE_MOTION,
	EVDEV_ABSOLUTE_TOUCH_UP,
	EVDEV_ABSOLUTE_MT_DOWN,
	EVDEV_ABSOLUTE_MT_MOTION,
	EVDEV_ABSOLUTE_MT_UP,
	EVDEV_RELATIVE_MOTION,
};

enum evdev_device_seat_capability {
	EVDEV_DEVICE_POINTER = (1 << 0),
	EVDEV_DEVICE_KEYBOARD = (1 << 1),
	EVDEV_DEVICE_TOUCH = (1 << 2)
};

struct mt_slot {
	int32_t seat_slot;
	int32_t x, y;
};

struct evdev_device {
	struct libinput_device base;

	struct libinput_source *source;

	struct evdev_dispatch *dispatch;
	struct libevdev *evdev;
	char *output_name;
	char *devnode;
	char *sysname;
	const char *devname;
	int fd;
	struct {
		int min_x, max_x, min_y, max_y;
		int32_t x, y;

		int32_t seat_slot;

		int apply_calibration;
		float calibration[6];
	} abs;

	struct {
		int slot;
		struct mt_slot *slots;
		size_t slots_len;
	} mt;
	struct mtdev *mtdev;

	struct {
		li_fixed_t dx, dy;
	} rel;

	enum evdev_event_type pending_event;
	enum evdev_device_seat_capability seat_caps;

	int is_mt;

	struct {
		struct motion_filter *filter;
	} pointer;
};

#define EVDEV_UNHANDLED_DEVICE ((struct evdev_device *) 1)

struct evdev_dispatch;

struct evdev_dispatch_interface {
	/* Process an evdev input event. */
	void (*process)(struct evdev_dispatch *dispatch,
			struct evdev_device *device,
			struct input_event *event,
			uint64_t time);

	/* Destroy an event dispatch handler and free all its resources. */
	void (*destroy)(struct evdev_dispatch *dispatch);
};

struct evdev_dispatch {
	struct evdev_dispatch_interface *interface;
};

struct evdev_device *
evdev_device_create(struct libinput_seat *seat,
		    const char *devnode,
		    const char *sysname);

struct evdev_dispatch *
evdev_touchpad_create(struct evdev_device *device);

struct evdev_dispatch *
evdev_mt_touchpad_create(struct evdev_device *device);

void
evdev_device_proces_event(struct libinput_event *event);

void
evdev_device_led_update(struct evdev_device *device, enum libinput_led leds);

int
evdev_device_get_keys(struct evdev_device *device, char *keys, size_t size);

const char *
evdev_device_get_output(struct evdev_device *device);

const char *
evdev_device_get_sysname(struct evdev_device *device);

void
evdev_device_calibrate(struct evdev_device *device, float calibration[6]);

int
evdev_device_has_capability(struct evdev_device *device,
			    enum libinput_device_capability capability);

li_fixed_t
evdev_device_transform_x(struct evdev_device *device,
			 li_fixed_t x,
			 uint32_t width);

li_fixed_t
evdev_device_transform_y(struct evdev_device *device,
			 li_fixed_t y,
			 uint32_t height);

void
evdev_device_remove(struct evdev_device *device);

void
evdev_device_destroy(struct evdev_device *device);

#endif /* EVDEV_H */
