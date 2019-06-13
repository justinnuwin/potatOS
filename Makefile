AS=nasm
ASFLAGS=-f elf64
CC=x86_64-elf-g++
CFLAGS=-ffreestanding -mno-red-zone -Wall -Wextra -g -c
LD=x86_64-elf-ld
LDFLAGS=-n -g
HOME=/home/cpe454/potatOS/

OBJECTS=multiboot_header.o boot.o long_mode_init.o kernel.o string.o vga.o printk.o ps2.o keyboard.o interrupt.o isr_wrapper.o generic_isr.o gdt.o serial.o multiboot2_tags.o page_table.o kmalloc.o threading.o

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
	sudo cp -r img/* boot 
	sudo umount boot 
	losetup -d /dev/loop9371
	losetup -d /dev/loop2816
	rm -r boot

kernel.bin: linker.ld $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ -T $^ 
	cp $@ img/boot

multiboot_header.o: multiboot_header.asm
	$(AS) $(ASFLAGS) $<

boot.o: boot.asm
	$(AS) $(ASFLAGS) $<

long_mode_init.o: long_mode_init.asm
	$(AS) $(ASFLAGS) $<

kernel.o: kernel.cpp kernel.h
	$(CC) -o $@ $(CFLAGS) $<

string.o: string.c string.h
	$(CC) -o $@ $(CFLAGS) $< 

vga.o: vga.cpp vga.h
	$(CC) -o $@ $(CFLAGS) $< 

printk.o: printk.cpp printk.h
	$(CC) -o $@ $(CFLAGS) $< 

ps2.o: ps2.cpp ps2.h
	$(CC) -o $@ $(CFLAGS) $<

keyboard.o: keyboard.cpp keyboard.h
	$(CC) -o $@ -Wno-narrowing $(CFLAGS) $< 

interrupt.o: interrupt.c interrupt.h
	$(CC) -o $@ $(CFLAGS) $<

isr_wrapper.o: isr_wrapper.asm
	$(AS) $(ASFLAGS) $<

generic_isr.o: generic_isr.c generic_isr.h
	$(CC) -o $@ $(CFLAGS) $<

gdt.o: gdt.c gdt.h
	$(CC) -o $@ $(CFLAGS) $<

serial.o: serial.cpp serial.h
	$(CC) -o $@ $(CFLAGS) $<

multiboot2_tags.o: multiboot2_tags.c multiboot2_tags.h
	$(CC) -o $@ $(CFLAGS) $<

page_table.o: page_table.c page_table.h
	$(CC) -o $@ $(CFLAGS) $<

kmalloc.o: kmalloc.c kmalloc.h
	$(CC) -o $@ $(CFLAGS) $<

threading.o: threading.c threading.h
	$(CC) -o $@ $(CFLAGS) $<

.PHONY: clean

clean:
	rm -f *.o
	rm -f *.bin
	rm -f *.img
	rm -rf boot
