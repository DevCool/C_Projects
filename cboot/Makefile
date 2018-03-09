CFLAGS=-Wall
LDFLAGS=-static --oformat=binary --nmagic

.PHONY: all clean disk deploy run
all: deploy

clean:
	rm -f *~ *.o *.out boot.bin io.sys c.img

boot.o: boot.c
	$(CC) $(CFLAGS) -Os -march=i686 -nostdlib -nostartfiles -nodefaultlibs -nostdinc -ffreestanding -m16 -c -o $@ $^

io.o: io.c
	$(CC) $(CFLAGS) -Os -march=i686 -nostdlib -nostartfiles -nodefaultlibs -nostdinc -ffreestanding -m16 -c -o $@ $^

boot: boot.o
	$(LD) $(LDFLAGS) -m elf_i386 -T boot.ld -o $@.bin $^

io: io.o
	$(LD) $(LDFLAGS) -m elf_i386 -T io.ld -o $@.sys $^

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
