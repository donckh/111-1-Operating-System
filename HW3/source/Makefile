APP = kfetch
obj-m+=${APP}.o
all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
	$(CC) test.c -o test
	$(CC) testkfetch.c -o testkfetch
clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
	rm test
load:
	sudo insmod ${APP}.ko
unload:
	sudo rmmod ${APP}
