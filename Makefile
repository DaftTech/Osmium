MODS = $(dir $(shell find modules/ -name 'Makefile'))
ALL  = $(dir $(shell find -mindepth 2 -name 'Makefile'))

.PHONY: all
all:
	make kernel || exit 1
	make modules || exit 1

	-rm bin/kernel.objdump
	objdump -dS bin/kernel > bin/kernel.objdump
	-rm bin/initrfs/kernel
	cp bin/kernel bin/initrfs/kernel

	-rm bin/initrfs.tar
	cd bin/initrfs; tar -cWf ../initrfs.tar *

	make clean

.PHONY: kernel
kernel:
	make -C lib-common/ -B || exit 1
	make -C kernel/ -B || exit 1
	make -C lib-osmium/ -B || exit 1

.PHONY: modules
modules: $(MODS)
	$(foreach m,$^,make -C $(m) -B || exit 1;)

.PHONY: clean
clean: $(ALL)
	$(foreach m,$^,make -C $(m) clean;)

.PHONY: run
run:
	qemu-system-i386 -kernel bin/kernel -initrd bin/initrfs.tar -monitor telnet:127.0.0.1:1234,server,nowait -m 1024 --no-reboot --no-shutdown
