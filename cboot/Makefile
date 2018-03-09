CFLAGS=-Wall
LDFLAGS=-melf_i386 -static -nostdlib -nostartfiles -nobuiltin --nmagic

.PHONY: all clean disk deploy
all: deploy

clean:
	rm -rf *~ *.o *.out boot.bin io.sys c.img

boot.o: boot.c
	$(CC) $(CFLAGS) -Os -march=i686 -ffreestanding -m16 -c -o $@ $^

io.o: io.c
	$(CC) $(CFLAGS) -Os -march=i686 -ffreestanding -m16 -c -o $@ $^

boot: boot.o
	$(LD) $(LDFLAGS) -T boot.ld -o $@.out $^
	objcopy -O binary $@.out $@.bin

io: io.o
	$(LD) $(LDFLAGS) -T io.ld -o $@.out $^
	objcopy -O binary $@.out $@.sys

disk:
	dd if=/dev/zero of=c.img bs=512 count=2880
	sudo losetup /dev/loop0 c.img
	sudo mkfs.vfat /dev/loop0
	sudo losetup -d /dev/loop0

deploy: disk boot io
	dd if=boot.bin of=c.img bs=1 count=512 conv=notrunc
	dd if=io.sys of=c.img bs=1 seek=512 conv=notrunc

run: deploy
	qemu-system-i386 -fda c.img -boot a
