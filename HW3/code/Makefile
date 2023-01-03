#
# Makefile by Don 
#
obj-m       += hello.o
KVERSION := $(shell uname -r)

all:
	$(MAKE) -C /lib/modules/$(KVERSION)/build M=$(PWD) modules

clean:
	$(MAKE) -C /lib/modules/$(KVERSION)/build M=$(PWD) clean
	
load:
	sudo insmod hello.ko

unload:	
	sudo rmmod hello
