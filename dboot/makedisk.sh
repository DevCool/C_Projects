#!/bin/sh
dd if=/dev/zero of=floppy.img bs=512 count=2880 || exit 1
dd if=stage1.bin of=floppy.img bs=512 count=1 conv=notrunc || exit 1
