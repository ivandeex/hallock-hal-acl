# vi: set ts=4 sw=4
# Hallock
# Makefile for HAL tool
#
# $Id$
# Copyright (C) 2008-2011, vitki.net
#

CP = /bin/cp -f
RM = /bin/rm -f
SUDO=/usr/bin/sudo
SUCP = $(SUDO) $(CP)
SURM = $(SUDO) $(RM)

help:
	echo "usage: make compile|prepare|install|uninstall|clean|test"

APT_GET = /usr/bin/apt-get
APT_INSTALL = $(APT_GET) install -y
HAL_INCLUDES = -I /usr/include/dbus-1.0 -I /usr/include/hal -I /usr/include/glib-2.0 -I /usr/lib/glib-2.0/include -I /usr/lib/dbus-1.0/include -I /usr/include/PolicyKit
HAL_LIBDIR=/usr/lib/hal
HAL_ALLCCOPTS = -Wall -O2 $(HAL_INCLUDES)
DEBUG = 0

hal-acl-tool-readonly: hal-acl-tool-readonly.c
	gcc $(HAL_ALLCCOPTS) -ldbus-1 -lhal -lpolkit -lglib-2.0 -o $@ $<

hal-addon-custom-skin: hal-addon-custom-skin.c
	gcc $(HAL_ALLCCOPTS) -DDEBUG=$(DEBUG) -ldbus-1 -lhal -o $@ $<

prepare:
	$(APT_INSTALL) libdbus-1-dev libglib2.0-dev libhal-dev libpolkit-dev

compile: hal-acl-tool-readonly hal-addon-custom-skin

clean:
	-$(RM) hal-acl-tool-readonly hal-addon-custom-skin hal-*.o

remake: clean compile

install: compile
	$(SUCP) hal-addon-custom-skin $(HAL_LIBDIR)/
	$(SUCP) hal-acl-tool-readonly $(HAL_LIBDIR)/
	$(SUCP) 99-hallock-policy.fdi /etc/hal/fdi/policy/
	-$(SUDO) /etc/init.d/hal restart

uninstall:
	-$(SURM) $(HAL_LIBDIR)/hal-addon-custom-skin
	-$(SURM) $(HAL_LIBDIR)/hal-acl-tool-readonly
	-$(SURM) /etc/hal/fdi/policy/99-hallock-policy.fdi
	-$(SUDO) /etc/init.d/hal restart

test:

