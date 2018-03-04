SRCS = $(shell find -name '*.c')
SRCS += $(shell find -name '*.cpp')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

CC = gcc -std=gnu99 -fno-leading-underscore
CP = g++ -fno-leading-underscore -std=c++0x -fno-exceptions -O1 -fpermissive
AR = ar

ASFLAGS = -m32
CFLAGS = -m32 -Wall -Werror  -Wno-unused -Wno-write-strings -g -fno-builtin -fno-stack-protector -nostdinc -I include -Os
ARFLAGS = rcs

lib-common.a: $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

%.o: %.c
	$(CP) $(CFLAGS) -c -o $@ $^
	
%.o: %.cpp
	$(CP) $(CFLAGS) -c -o $@ $^

clean:
	-rm $(OBJS)

.PHONY: clean