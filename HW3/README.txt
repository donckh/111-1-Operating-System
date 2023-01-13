https://hackmd.io/@Cycatz/ryiENe4Oj

Install cmake:
https://segmentfault.com/a/1190000021911081

1. sudo apt install cmake

2. download linux_x86_64 tar.gz
https://link.segmentfault.com/?enc=aGYiyx67qBrpvjKo3dIa1Q%3D%3D.X4fT2L46sZLWVqCDoQfuhUbPhc51Tf7oOgDbxm%2FFhZ8%3D

3. unzip and cd to bin
tar -zxvf cmake-3.17.0-rc2-Linux-x86_64.tar.gz
cd cmake-3.17.0-rc2-Linux-x86_64/bin

4. check version
cmake -version

5. build .cpp and CMakeLists.txt
A-hello-cmake$ tree
.
├── CMakeLists.txt
├── main.cpp

6. cmake to build makefile
cmake ./CMakeLists.txt

7. build object file
make

8. execute hello_cmake
./hello_cmake

-----------------------------
1. use below command in .cpp to sethostname and gethostname, implement sudo ./main.o for set hostname:
sethostname("310551003-Virtualbox", 20);
gethostname(hostname, 100);

2. build module
3. insmod to load module
4. rmmode to unload module


start_kernel()->rest_init()->kernel_init()->kernel_init_freeable()->do_basic_setup()->do_initcalls()

reference:
https://github.com/volatilityfoundation/volatility/issues/812
https://blog.wu-boy.com/2010/07/linux-kernel-%E7%B0%A1%E5%96%AE-hello-world-license-and-module-%E4%BB%8B%E7%B4%B9part-3/
https://meetonfriday.com/posts/c4426b79/
https://ithelp.ithome.com.tw/users/20001007/ironman/958

------------------------------

hello world kernel module:
https://ithelp.ithome.com.tw/users/20001007/ironman/958

1. create hello.c file
#include <linux/module.h>    /* Needed by all modules */
#include <linux/kernel.h>    /* Needed for KERN_INFO */

int init_module(void)
{
	printk(KERN_INFO "Hello world 1.\n");
	/* 
	* A non 0 return means init_module failed; module can't be loaded. 
	*/
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "Goodbye world 1.\n");
}

MODULE_LICENSE("GPL");

2. create Makefile
#
# Makefile by Don 
#
obj-m       += hello.o
KVERSION := $(shell uname -r)

all:
	$(MAKE) -C /lib/modules/$(KVERSION)/build M=$(PWD) modules

clean:
	$(MAKE) -C /lib/modules/$(KVERSION)/build M=$(PWD) clean
	
	
3. on the save directory of .c and Makefile, type make:
make

4. check .ko file 
modinfo kfetch.ko

5. initial hello.ko
sudo insmod kfetch.ko

6. check module exist or not
lsmod | grep kfetch
cat /proc/modules | grep kfetch

7. check syslog message, verify kernel message
less /var/log/syslog | grep kfetch

8. remove module, check module remove or not
sudo rmmod kfetch
lsmod | grep kfetch
cat /proc/modules | grep kfetch

9. check kernel message
less /var/log/syslog | grep hello
less /var/log/syslog | grep bye

10. check directory
ls /dev/kfetch

reference: write read string
https://github.com/derekmolloy/exploringBB/blob/master/extras/kernel/ebbchar/testebbchar.c

include:
https://blog.csdn.net/weixin_34067980/article/details/93975353

kernel method:
https://stackoverflow.com/questions/27338813/how-to-print-linux-kernel-version-number-in-kernel-module

TA feedback:
segfaults when running the client & cannot set the mask (-15)
Ans: To be written a bitwise function connect with option command
