CC=i686-linux-musl-gcc
AS=i686-linux-musl-as
LD=i686-linux-musl-gcc

CFLAGS=-ffreestanding -O2 -std=gnu99 -Iinclude -Wall -Wextra -Werror -mgeneral-regs-only
AFLAGS=
LFLAGS=-ffreestanding -nostdlib -lgcc -O2

FILES=codes/boot.o codes/kernel.o codes/io.o codes/keyboard.o codes/gdt.o codes/lgdt.o codes/string.o codes/terminal.o
HEADERS=include/keyboard.h include/io.h include/string.h include/gdt.h include/terminal.h
OSNAME=myos

.PHONY: all
all: $(OSNAME).iso

$(OSNAME).iso: $(OSNAME).bin grub.cfg
	mkdir -p isodir/boot/grub; \
	cp grub.cfg isodir/boot/grub; \
	cp $(OSNAME).bin isodir/boot; \
	grub-mkrescue -o $(OSNAME).iso isodir

$(OSNAME).bin: $(FILES) $(HEADERS) linker.ld
	$(LD) $(LFLAGS) -T linker.ld -o $(OSNAME).bin $(FILES)

%.o: %.S
	$(AS) $(AFLAGS) -o $@ $<

%.o: %.s
	$(AS) $(AFLAGS) -o $@ $<

%.s: %.c
	$(CC) $(CFLAGS) -S -o $@ $<

.PHONY: unmake
unmake: clean
	@if [ -f "$(OSNAME).bin" ]; then rm $(OSNAME).bin; fi
	@if [ -f "$(OSNAME).iso" ]; then rm $(OSNAME).iso; fi
	
.PHONY: clean
clean:
	@if [ -d isodir ]; then rm -rf isodir; fi
	@find . -regextype posix-extended -regex '(.*\.s)|(.*\.o)' -delete
