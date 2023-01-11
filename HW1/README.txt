https://hackmd.io/yJfXqJKnSy2_liuIOiD4Hg

1. download kernel file and install dependence
sudo apt-get install git fakeroot build-essential ncurses-dev xz-utils libssl-dev bc flex libelf-dev bison vim gcc make

2. make dir and cd to there
mkdir ~/Don/kernelbuild/
mv ~/Downloads/linux-5.19.12.tar.xz  ~/Don/kernelbuild
cd Don/kernelbuild/
unxz linux-5.19.12.tar.xz 
tar xvf linux-5.19.12.tar 

3. change authorize
chown -R don:don linux-5.19.12

4. make config file, copy current linux release(uname -r) config file to our kenerl build file
cd linux-5.19.12/
make mrproper
cp -v /boot/config-$(uname -r) .config

5. make config file
make menuconfig

6. change version, CONFIG_LOCALVERSION to change suffix, 
CONFIG_SYSTEM_TRUSTED_KEYS a PEM-encoded file for additional certificates, CONFIG_SYSTEM_REVOCATION_KEYS include X.509 certificates
vim .config
/\<CONFIG_LOCALVERSION\>
# change "" to "-os-id"
CONFIG_LOCALVERSION = "-os-id"
CONFIG_SYSTEM_TRUSTED_KEYS=""
CONFIG_SYSTEM_REVOCATION_KEYS=""

7. check version before execute, CONFIG_LOCALVERSION_AUTO=y means add suffix
sudo apt install --reinstall linux-headers-$(uname -r)
make oldconfig && make prepare
make CONFIG_LOCALVERSION_AUTO=y kernelrelease
# should be show what you changed

8. install dependence
make -j 4

9. install kernel
sudo make modules_install
sudo make install

10. install grub properly
sudo grub-install /dev/sda
sudo update-grub

11. restart and check version
uname -a
cat /etc/os-release

Part 2:
1. add syscall table
vim ~Don/kernelbuild/linux-5.19.12/arch/x86/entry/syscalls/syscall_64.tbl
548	common	hello			sys_hello


2. add system call at the bottom of file before #endif
vim include/linux/syscalls.h
asmlinkage long sys_hello(void);
#endif

3. add syscall for hello
cd ~/Don/kernelbuild/linux-5.19.12/
mkdir hello
vim hello/hello.c

#include <linux/kernel.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE0(hello)
{
	printk("Hello, world!\n");
	printk("310551003\n");
	return 0;
}

4. add hello makefile
vim hello/Makefile
obj-y := hello.o

5. edit makefile
vim Makefile
/core-y
core-y			+= kernel/ certs/ mm/ fs/ ipc/ security/ crypto/ hello/

6. goto run part 1 step 7 run it til the end

7. create .cpp code for call system code:
cd ~/Don
vim hw1-1.cpp
#include <assert.h>
#include <unistd.h>
#include <sys/syscall.h>

/*
 * You must copy the __NR_hello marco from
 * <your-kernel-build-dir>/arch/x86/include/generated/uapi/asam/unistd_64.h
 * In this example, the value of __NR_hello is 548
 */
#define __NR_hello 548

int main(int argc, char *argv[]) {
    int ret = syscall(__NR_hello);
    assert(ret == 0);

    return 0;
}


8. check message from hello:
sudo dmesg | grep Hello

Part 3:
1. restart computer and go to original linux kernel
uname -a
cat /etc/os-release

2. add syscall table
vim ~Don/kernelbuild/linux-5.19.12/arch/x86/entry/syscalls/syscall_64.tbl
549	common	revstr			sys_revstr.c


3. add system call at the bottom of file before #endif
vim include/linux/syscalls.h
asmlinkage long sys_revstr(int str_len, char __user *input_str);
#endif


4. add syscall for hello
cd ~/Don/kernelbuild/linux-5.19.12/
mkdir revstr
vim revstr/revstr.c

#include <linux/kernel.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE2(revstr, int, len, char __user *, str)
{
	// copy from user space to kernel buff
	//char buf[256], rev[256];
	//unsigned long lenleft = len;
	//unsigned long chunklen = sizeof(buf);
	/*
	while( lenleft > 0 ){
		if( lenleft < chunklen ) chunklen = lenleft;
		if( copy_from_user(buf, str, chunklen) ){
		return -EFAULT;
		}
		lenleft -= chunklen;
	}
	*/
	char buf[256], nor[256], rev[256];
	if(copy_from_user(buf, str, len)) return -EFAULT;
	for(int i=len-1, j=0; i>=0 ; i--, j++){
		rev[j] = buf[i];
		nor[j] = buf[j];
		printk("rev[j] nor[j]\n");
	}
	printk("before\n");
	printk("The origin string: %s\n", nor);
	printk("The reversed string: %s\n", rev);
	rev[len] = '\0';
	nor[len] = '\0';
	printk("The origin string: %s\n", nor);
	printk("The reversed string: %s\n", rev);
	printk("updated\n");
	return 0;
}

5. add hello makefile
vim revstr/Makefile
obj-y := revstr.o

6. edit makefile
vim Makefile
/core-y
core-y			+= kernel/ certs/ mm/ fs/ ipc/ security/ crypto/ hello/ revstr/

7. goto run part 1 step 7 run it til the end

8. create .cpp code for call system code:
cd ~/Don
vim hw1-2.cpp
#include <assert.h>
#include <unistd.h>
#include <sys/syscall.h>

/*
 * You must copy the __NR_revstr marco from
 * <your-kernel-build-dir>/arch/x86/include/generated/uapi/asam/unistd_64.h
 * In this example, the value of __NR_revstr is 549
 */
#define __NR_revstr 549

int main(int argc, char *argv[]) {  
    int ret1 = syscall(__NR_revstr, 5, "hello");
    assert(ret1 == 0);

    int ret2 = syscall(__NR_revstr, 11, "5Y573M C411");
    assert(ret2 == 0);

    return 0;
}

9. run .cpp and execute object file
cd ~/Don
g++ -o hw ./hw1-2.cpp
./hw


10. check message from dmesg:
sudo dmesg | grep origin
sudo dmesg | grep reversed


Remark:
1. check space:
df -h
dpkg --get-selections | grep linux-image
