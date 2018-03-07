CFLAGS=-Wall

.PHONY: all clean disk deploy
all: boot io disk deploy

clean:
	rm -rf *~ *.o *.elf boot io.sys c.img

boot.o: boot.c
	$(CC) $(CFLAGS) -Os -m32 -c -o $@ $^

io.o: io.c
	$(CC) $(CFLAGS) -O -fno-builtin -m32 -c -o $@ $^

boot: boot.o
	$(LD) $(LDFLAGS) -T boot.ld -melf_i386 -o $@.elf $^
	objcopy -O binary $@.elf $@

io: io.o
	$(LD) $(LDFLAGS) -T io.ld -melf_i386 -o $@.elf $^
	objcopy -O binary $@.elf $@.sys

disk:
	dd if=/dev/zero of=c.img bs=512 count=2880
	sudo losetup /dev/loop0 c.img
	sudo mkfs.vfat /dev/loop0
	sudo losetup -d /dev/loop0

deploy: disk boot io
	dd if=boot of=c.img bs=1 count=512 conv=notrunc
	dd if=io.sys of=c.img bs=1 seek=512 conv=notrunc
