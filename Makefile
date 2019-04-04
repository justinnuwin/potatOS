AS=nasm
ASFLAGS=-f elf64
LD=x86_64-elf-ld
LDFLAGS=-n 
HOME=/home/cpe454/potatOS/

OBJECTS=multiboot_header.o boot.o

potatOS.img: kernel.bin
	dd if=/dev/zero of=$@ bs=512 count=32768
	parted $@ mklabel msdos
	parted $@ mkpart primary fat32 2048s 30720s
	parted $@ set 1 boot on
	losetup /dev/loop9371 $@
	losetup /dev/loop2816 $@ -o 1048576
	mkdosfs -F32 -f 2 /dev/loop2816
	mkdir -p boot
	sudo mount /dev/loop2816 boot 
	sudo grub-install --root-directory=$(HOME)boot --no-floppy --modules="normal part_msdos multiboot" /dev/loop9371
	sudo cp -r .img/* boot 
	sudo umount boot 
	losetup -d /dev/loop9371
	losetup -d /dev/loop2816
	rm -r boot

kernel.bin: $(OBJECTS) linker.ld
	$(LD) $(LDFLAGS) -o $@ -T linker.ld $(OBJECTS)
	cp $@ .img/boot


multiboot_header.o: multiboot_header.asm
	$(AS) $(ASFLAGS) $<

boot.o: boot.asm
	$(AS) $(ASFLAGS) $<

.PHONY: clean

clean:
	rm -f *.o
	rm -f *.bin
	rm -f *.img
	rm -rf boot
