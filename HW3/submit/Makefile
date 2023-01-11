APP = kfetch_mod_310551003
obj-m+=${APP}.o
all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
	$(CC) kfetch.c -o kfetch
clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
	rm test
load:
	sudo insmod ${APP}.ko
unload:
	sudo rmmod ${APP}
