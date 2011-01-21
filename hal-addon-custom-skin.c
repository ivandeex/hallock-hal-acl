/***************************************************************************
 * vi: set ts=4 sw=4
 * Hallock
 *
 * hal-addon-custom-skin.c
 *
 * $Id$
 *
 * Copyright (C) 2008-2011, vitki.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 **************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>

#include <libhal.h>

#define DEBUGGING 0

#define UDI "/org/freedesktop/Hal/devices/computer"
#define KEY "hwguard.skin"

static LibHalContext *hal_ctx;

int
main (int argc, char *argv[])
{
	dbus_bool_t rc = 0;
	DBusError error;
	char cmdline[1024];
	int len;
	char *skin = "unknown";
	char *beg, *loc;

	int fh = open("/proc/cmdline", O_RDONLY);
	if (fh < 0) {
		fprintf(stderr, "error: cannot open /proc/cmdline");
		return 1;
	}

	len = read(fh, cmdline, sizeof cmdline);
	if (len < 0) {
		fprintf(stderr, "error: cannot read from /proc/cmdline");
		return 1;
	}

	close(fh);
	cmdline[len] = '\0';

	beg = cmdline;
	while (1) {
		loc = strstr(beg, "skin=");
		if (NULL == loc)
			break;
		if (loc == beg || *(loc - 1) == ' ') {
			loc += 5;
			if (*loc != '\0' && *loc != ' ') {
				skin = loc;
				while (*loc != '\0' && *loc != ' ' && *loc != '\t'
						&& *loc != '\r' && *loc != '\n' && *loc != '\b')
					loc++;
				*loc = '\0';
				break;
			}
		}
		beg = loc + 1;
	}

	dbus_error_init (&error);

#if DEBUGGING
	if ((hal_ctx = libhal_ctx_new ()) == NULL) {
		fprintf (stderr, "error: libhal_ctx_new\n");
		return 1;
	}
	if (!libhal_ctx_set_dbus_connection (hal_ctx, dbus_bus_get (DBUS_BUS_SYSTEM, &error))) {
		fprintf (stderr, "error: libhal_ctx_set_dbus_connection: %s: %s\n", error.name, error.message);
		LIBHAL_FREE_DBUS_ERROR (&error);
		return 1;
	}
	if (!libhal_ctx_init (hal_ctx, &error)) {
		if (dbus_error_is_set(&error)) {
			fprintf (stderr, "error: libhal_ctx_init: %s: %s\n", error.name, error.message);
			dbus_error_free (&error);
		}
		fprintf (stderr, "Could not initialise connection to hald.\n"
				 "Normally this means the HAL daemon (hald) is not running or not ready.\n");
		return 1;
	}
#else
	/* normal operation under hald */
	if ((hal_ctx = libhal_ctx_init_direct (&error)) == NULL) {
		fprintf (stderr, "%d: Cannot connect to hald: %s: %s\n",
				getpid(), error.name, error.message);
		LIBHAL_FREE_DBUS_ERROR (&error);
		return 1;
	}
#endif

	rc = libhal_device_set_property_string (hal_ctx, UDI, KEY, skin, &error);
	    
	return rc ? 0 : 1;
}

