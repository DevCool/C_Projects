#!/bin/sh
. ./build.sh && . ./makedisk.sh && qemu-system-i386 -fda floppy.img -boot a
