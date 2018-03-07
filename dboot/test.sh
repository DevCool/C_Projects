#!/bin/sh
. ./build.sh && . ./makedisk.sh
echo -e "Type 'bochs' with quotes to use bochs\nor anything else will run qemu.\n"
read input
if [ "$input" = "bochs" ]; then
	which bochs > /dev/null 2>&1 || { echo "Install bochs."; exit 1; }
	bochs -f bochsrc.txt
else
	which qemu-system-i386 > /dev/null 2>&1 || { echo "Install qemu i386."; exit 1; }
	qemu-system-i386 -fda floppy.img -boot a
fi
unset $input
