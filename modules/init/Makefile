SRCS = $(shell find -name '*.c')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

CC = i586-elf-gcc -std=gnu99 -fno-leading-underscore
LD = i586-elf-ld

ASFLAGS = -m32
CFLAGS = -m32 -Wall -g -fno-stack-protector -fno-builtin -nostdinc -Iinclude -I../../lib-osmium/include
LDFLAGS = -melf_i386 -Tlink.ld -u _start --gc-sections 

../../initrfs/init: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^ ../../lib-osmium/lib-osmium.a
	objdump -dSx $@ > mod.objdump

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm $(OBJS)

.PHONY: clean