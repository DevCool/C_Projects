#!/bin/sh
gcc -Wl,--oformat=binary -fno-pie -nostdlib -fomit-frame-pointer -fno-builtin -nostartfiles -nodefaultlibs -Wl,-T,stage1.ld -Os -std=c89 -m16 stage1.c -o stage1.bin || exit 1
