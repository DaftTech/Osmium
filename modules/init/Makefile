SRCS = $(shell find -name '*.c')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

CC = gcc -std=gnu99 -fno-leading-underscore
LD = ld

ASFLAGS = -m32
CFLAGS = -m32 -Wall -g -fno-stack-protector -fno-builtin -nostdinc -Iinclude -I../../lib-osmium/include
LDFLAGS = -melf_i386 -Tlink.ld -u _start --gc-sections 

../../bin/initrfs/init: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^ ../../lib-osmium/lib-osmium.a
	objdump -dSx $@ > mod.objdump

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm $(OBJS)

.PHONY: clean