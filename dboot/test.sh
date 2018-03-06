#!/bin/sh
. ./build.sh && . ./makedisk.sh
echo -e "Type 'qemu' with quotes to use qemu\nor anything else will run bochs.\n"
read input
if [ "$input" = "qemu" ]; then
	which qemu-system-i386 > /dev/null 2>&1 || { echo "Install qemu i386."; exit 1; }
	qemu-system-i386 -fda floppy.img -boot a
else
	which bochs > /dev/null 2>&1 || { echo "Install bochs."; exit 1; }
	bochs -f bochsrc.txt
fi
unset $input
