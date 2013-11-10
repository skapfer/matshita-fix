simple_flash.x64: lscsi.c simple_flash.c xscsi.h
	gcc -static -o simple_flash.x64 lscsi.c simple_flash.c
	gcc -m32 -static -o simple_flash.x86 lscsi.c simple_flash.c
	strip simple_flash.x??
