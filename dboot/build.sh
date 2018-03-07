#!/bin/sh
gcc -m32 -c -Os -march=i686 -ffreestanding -Wall -Werror -o stage1.o stage1.c || exit 1
ld -melf_i386 -static -Tstage1.ld -nostdlib --nmagic -o stage1.elf stage1.o || exit 1
objcopy -O binary stage1.elf stage1.bin || exit 1
