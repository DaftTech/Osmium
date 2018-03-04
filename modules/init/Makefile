SRCS = $(shell find -name '*.c')
SRCS += $(shell find -name '*.cpp')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

CC = gcc -std=gnu99 -fno-leading-underscore
CP = g++ -fno-leading-underscore -std=c++0x -fno-exceptions -O1 -fpermissive
LD = ld

ASFLAGS = -m32
CFLAGS = -m32 -Wall -Werror  -g -Wno-write-strings -fno-stack-protector -fno-builtin -nostdinc -Iinclude -I../../lib-osmium/include -I../../lib-common/include
LDFLAGS = -melf_i386 -Tlink.ld -u _start --gc-sections

../../bin/initrfs/init: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^ ../../lib-osmium/lib-osmium.a ../../lib-common/lib-common.a
	objdump -dSx $@ > mod.objdump

%.o: %.c
	$(CP) $(CFLAGS) -c -o $@ $^
	
%.o: %.cpp
	$(CP) $(CFLAGS) -c -o $@ $^

clean:
	rm $(OBJS)

.PHONY: clean