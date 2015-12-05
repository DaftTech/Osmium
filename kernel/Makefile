SRCS  = $(shell find -name '*.S')
SRCS += $(shell find -name '*.c')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

CC = i586-elf-gcc -std=gnu99 -fno-leading-underscore -O1
LD = i586-elf-ld

ASFLAGS = -m32
CFLAGS = -m32 -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -Werror -g -fno-stack-protector -nostdinc -I ./include
LDFLAGS = -melf_i386 -T kernel.ld

../bin/kernel: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^
	
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

%.o: %.S
	$(CC) $(ASFLAGS) -c -o $@ $^

.PHONY: clean
clean:
	rm $(OBJS)