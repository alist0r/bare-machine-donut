gcc = i686-elf-gcc
CPARAMS = -g -ffreestanding -Wall
LDPARAMS = -melf_i386
NASMPARAMS = -f elf32
objects = start.o vga.o main.o 
%.o: %.c
	$(gcc) $(CPARAMS) -o $@ -c $<

%.o: %.asm
	nasm $(NASMPARAMS) -o $@ $<

boot.bin:
	nasm -f bin boot.asm -o boot.bin 


stage1.bin: linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o $@ $(objects)

img:
	dd if=/dev/zero of=donut.img bs=512 count=2880
	dd if=boot.bin of=donut.img conv=notrunc bs=512 seek=0 count=1
	dd if=stage1.bin of=donut.img conv=notrunc bs=512 seek=1 count=2048
all:
	make boot.bin
	make stage1.bin
	make img
clean:
	rm *.bin
	rm *.o
run:
	qemu-system-x86_64 -hdd donut.img
