#!/bin/sh
which qemu-system-i386 > /dev/null 2>&1
if [ ! "$?" = "0" ]; then
	echo "qemu-system-i386 is not installed."
	exit 1
fi
qemu-system-i386 -fda c.img -boot a
