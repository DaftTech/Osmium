SRCS  = $(shell find -name '*.S')
SRCS += $(shell find -name '*.c')
SRCS += $(shell find -name '*.cpp')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

CC = gcc -std=gnu99 -fno-leading-underscore -O1
CP = g++ -fno-leading-underscore -std=c++0x -fno-exceptions -O1 -fpermissive
LD = ld

ASFLAGS = -m32
CFLAGS = -m32 -Wall -Werror -Wextra -Wno-unused-variable -include ../lib-common/include/stdc.hpp -Wno-write-strings -Wno-unused-parameter -g -fno-stack-protector -nostdinc -I ./include -I ../lib-common/include
LDFLAGS = -melf_i386 -T kernel.ld

../bin/kernel: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^ ../lib-common/lib-common.a
	
%.o: %.c
	$(CP) $(CFLAGS) -c -o $@ $^
	
%.o: %.cpp
	$(CP) $(CFLAGS) -c -o $@ $^

%.o: %.S
	$(CC) $(ASFLAGS) -c -o $@ $^

.PHONY: clean
clean:
	rm $(OBJS)