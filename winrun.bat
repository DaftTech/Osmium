start "" qemu/qemu-system-i386.exe -kernel bin/kernel -initrd bin/initrfs.tar -monitor telnet:127.0.0.1:1234,server,nowait -m 1024 --no-reboot --no-shutdown
puttytel -telnet 127.0.0.1 1234