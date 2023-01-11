#include <linux/init.h>			// Macros used to mark up functions e.g. __init __exit
#include <linux/module.h>		// Core header for loading LKMs into the kernel
#include <linux/device.h>		// Header to support the kernel Driver Model
#include <linux/kernel.h>		// Contains types, macros, functions for the kernel
#include <linux/fs.h>			// Header for the Linux file system support
#include <linux/uaccess.h>		// Required for the copy to user function
#include <linux/string.h>
#include <linux/unistd.h>		//hostname
#include <linux/utsname.h>		//uname
#include <linux/cpufreq.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/mm.h>
#include <linux/swap.h>
#include <linux/mman.h>
//#include <linux/meminfo.h>
#define  DEVICE_NAME "kfetch"		///< The device will appear at /dev/kfetch using this value
#define  CLASS_NAME  "kf"		//< The device class -- this is a character device driver

MODULE_LICENSE("GPL");			///< The license type -- this affects available functionality
MODULE_AUTHOR("Don Chan 310551003");	///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("A simple Linux char driver for the HW3");	///< The description -- see modinfo
MODULE_VERSION("0.1");			///< A version number to inform users

static int    majorNumber;		///< Stores the device number -- determined automatically
static char   message[256] = {0};	///< Memory for the string that is passed from userspace
static short  size_of_message;		///< Used to remember the size of the string stored
static int    numberOpens = 0;		///< Counts the number of times the device is opened
static struct class*  kfetchClass  = NULL;	///< The device-driver class struct pointer
static struct device* kfetchDevice = NULL;	///< The device-driver device struct pointer

// The prototype functions for the character driver -- must come before the struct definition
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops =
{
	.open = dev_open,
	.read = dev_read,
	.write = dev_write,
	.release = dev_release,
};

/** @brief The LKM initialization function
 *  The static keyword restricts the visibility of the function to within this C file. The __init
 *  macro means that for a built-in driver (not a LKM) the function is only used at initialization
 *  time and that it can be discarded and its memory freed up after that point.
 *  @return returns 0 if successful
 */
static int __init kfetch_init(void){
	printk(KERN_INFO "kfetch: Initializing the kfetch LKM\n");

	// Try to dynamically allocate a major number for the device -- more difficult but worth it
	majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
	if (majorNumber<0){
		printk(KERN_ALERT "kfetch failed to register a major number\n");
		return majorNumber;
	}
	printk(KERN_INFO "kfetch: registered correctly with major number %d\n", majorNumber);

	// Register the device class
	kfetchClass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(kfetchClass)){                // Check for error and clean up if there is
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "Failed to register device class\n");
		return PTR_ERR(kfetchClass);          // Correct way to return an error on a pointer
	}
	printk(KERN_INFO "kfetch: device class registered correctly\n");

	// Register the device driver
	kfetchDevice = device_create(kfetchClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
	if (IS_ERR(kfetchDevice)){               // Clean up if there is an error
		class_destroy(kfetchClass);           // Repeated code but the alternative is goto statements
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "Failed to create the device\n");
		return PTR_ERR(kfetchDevice);
	}
	printk(KERN_INFO "kfetch: device class created correctly\n"); // Made it! device was initialized
	return 0;
}

/** @brief The LKM cleanup function
 *  Similar to the initialization function, it is static. The __exit macro notifies that if this
 *  code is used for a built-in driver (not a LKM) that this function is not required.
 */
static void __exit kfetch_exit(void){
	device_destroy(kfetchClass, MKDEV(majorNumber, 0));     // remove the device
	class_unregister(kfetchClass);                          // unregister the device class
	class_destroy(kfetchClass);                             // remove the device class
	unregister_chrdev(majorNumber, DEVICE_NAME);             // unregister the major number
	printk(KERN_INFO "kfetch: Goodbye from the LKM!\n");
}

/** @brief The device open function that is called each time the device is opened
 *  This will only increment the numberOpens counter in this case.
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_open(struct inode *inodep, struct file *filep){
	numberOpens++;
	printk(KERN_INFO "kfetch: Device has been opened %d time(s)\n", numberOpens);
	
	return 0;
}

/** @brief This function is called whenever device is being read from user space i.e. data is
 *  being sent from the device to the user. In this case is uses the copy_to_user() function to
 *  send the buffer string to the user and captures any errors.
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 *  @param buffer The pointer to the buffer to which this function writes the data
 *  @param len The length of the b
 *  @param offset The offset if required
 */
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
	int error_count = 0;
	
	//initialize parameter
	char new_message[512], sep[20] = "---------------", cpu_name[50], mem_total[20], mem_free[20];
	char str11[10]="", str12[30]="", str21[10]="", str22[20]="", str31[10]="", str32[50]="";
	char str41[10]="", str42[30]="", str51[10]="", str52[20]="", str61[10]="", str62[20]="";
	int mask;
	//cpu init
	unsigned int cpu = 0;
	struct cpuinfo_x86 *c;
	//mem init
	struct sysinfo i;
	unsigned long committed;
	long cached;
	long available;
	unsigned long pages[NR_LRU_LISTS];
	unsigned long sreclaimable, sunreclaim;
	int lru;
	//procs init
	struct task_struct* task_list;
	size_t process_counter = 0;
	//uptime init
	struct timespec64 uptime;
	unsigned long mins;
	
	sscanf(message, "%d", &mask);  //get mask from write

	
	// get cpu info
	for_each_online_cpu(cpu){
		const char *vendor = NULL;
		c = &cpu_data(cpu);
		if (c->x86_vendor < X86_VENDOR_NUM) {
			vendor = "Unknown";
		} 
		else {
			if (c->cpuid_level >= 0) vendor = c->x86_vendor_id;
		}

		if (vendor && !strstr(c->x86_model_id, vendor)){
			pr_cont("%s ", vendor);
			//printk(KERN_INFO "kfetch cpu vendor: %s ", vendor);
			//sprintf(cpu_name, "%s", vendor);
		}
		if (c->x86_model_id[0]){
			pr_cont("%s", c->x86_model_id);
			//printk(KERN_INFO "kfetch cpu model: %s", c->x86_model_id);
			sprintf(cpu_name, "%s", c->x86_model_id);
		}else{
			pr_cont("%d86", c->x86);
			//printk(KERN_INFO "kfetch cpu model: %d86", c->x86);
			sprintf(cpu_name, "%d86", c->x86);
		}
	}
	
	// get memory info
	si_meminfo(&i);
	committed = vm_memory_committed();
	cached = global_node_page_state(NR_FILE_PAGES) - total_swapcache_pages() - i.bufferram;
	if (cached < 0)
		cached = 0;

	for (lru = LRU_BASE; lru < NR_LRU_LISTS; lru++)
		pages[lru] = global_node_page_state(NR_LRU_BASE + lru);

	available = si_mem_available();
	sreclaimable = global_node_page_state_pages(NR_SLAB_RECLAIMABLE_B);
	sunreclaim = global_node_page_state_pages(NR_SLAB_UNRECLAIMABLE_B);

	//printk(KERN_INFO "kfetch totalram: %lu", i.totalram);
	//printk(KERN_INFO "kfetch freeram: %lu", i.freeram);
	sprintf(mem_total, "%lu MB", i.totalram * 4 / 1024);
	sprintf(mem_free, "%lu MB", i.freeram * 4 / 1024);
	
	// get num of processes
	for_each_process(task_list){
		++process_counter;
	}
	
	// get system uptime
	ktime_get_boottime_ts64(&uptime);
	mins = (unsigned long) uptime.tv_sec / 60;  // convert to mins
	
	// store string for printing according to mask
	if (mask & (1 << 0)) 
	{	
		strcpy(str11, "Kernel: ");
		strcpy(str12, utsname()->release);
		printk(KERN_INFO "kfetch kernel:  %s\n", str12);
	}
	if (mask & (1 << 1)) 
	{	
		strcpy(str31, "CPUs: "); 
		sprintf(str32, "%d / %d", num_active_cpus(), num_online_cpus());
		printk(KERN_INFO "kfetch CPUs name:  %s\n", str32);
	}
	if (mask & (1 << 2))
	{	
		strcpy(str21, "CPU: "); 
		sprintf(str22, cpu_name);
		printk(KERN_INFO "kfetch CPU:  %s\n", str22);
	}
	if (mask & (1 << 3))
	{	
		strcpy(str41, "Mem: ");
		sprintf(str42, "%s / %s", mem_free, mem_total);
		printk(KERN_INFO "kfetch Mem:  %s\n", str42);
	}
	if (mask & (1 << 4)) 
	{	
		strcpy(str61, "Uptime: ");
		sprintf(str62, "%lu mins", mins);
		printk(KERN_INFO "kfetch Uptime:  %s\n", str62);
	}
	if (mask & (1 << 5))
	{	
		strcpy(str51, "Procs: ");
		sprintf(str52, "%zu", process_counter);
		printk(KERN_INFO "kfetch Procs:  %s\n", str52);
	}
	
	// offset string if it is not showing all info
	for(int i=0;i<6;i++){
		if(strlen(str11)==0 && strlen(str21)!=0){
			strcpy(str11, str21);
			strcpy(str12, str22);
			strcpy(str21, "");
			strcpy(str22, "");
		}
		if(strlen(str21)==0 && strlen(str31)!=0){
			strcpy(str21, str31);
			strcpy(str22, str32);
			strcpy(str31, "");
			strcpy(str32, "");
		}
		if(strlen(str31)==0 && strlen(str41)!=0){
			strcpy(str31, str41);
			strcpy(str32, str42);
			strcpy(str41, "");
			strcpy(str42, "");
		}
		if(strlen(str41)==0 && strlen(str51)!=0){
			strcpy(str41, str51);
			strcpy(str42, str52);
			strcpy(str51, "");
			strcpy(str52, "");
		}
		if(strlen(str51)==0 && strlen(str61)!=0){
			strcpy(str51, str61);
			strcpy(str52, str62);
			strcpy(str61, "");
			strcpy(str62, "");
		}
	}
	
	//combine string
	sprintf(new_message, "                   %s\n"
			"        .-.        %s\n"
			"       (.. |       %-10s%s\n"
			"       <>  |       %-10s%s\n"
			"      / --- \\      %-10s%s\n"
			"     ( |   | |     %-10s%s\n"
			"   |\\\\_)___/\\)/\\   %-10s%s\n"
			"  <__)------(__/   %-10s%s\n", utsname()->nodename, sep,
			str11, str12, str21, str22, str31, str32, str41, str42, str51, str52, str61, str62);
	
	// copy_to_user has the format ( * to, *from, size) and returns 0 on success
	error_count = copy_to_user(buffer, new_message, strlen(new_message));

	if (error_count==0){            // if true then have success
		printk(KERN_INFO "kfetch: Sent %ld characters to the user\n", strlen(new_message));
		return (size_of_message=0);  // clear the position to the start and return 0
	}
	else {
		printk(KERN_INFO "kfetch: Failed to send %d characters to the user\n", error_count);
		return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
	}
}

/** @brief This function is called whenever the device is being written to from user space i.e.
 *  data is sent to the device from the user. The data is copied to the message[] array in this
 *  LKM using the sprintf() function along with the length of the string.
 *  @param filep A pointer to a file object
 *  @param buffer The buffer to that contains the string to write to the device
 *  @param len The length of the array of data that is being passed in the const char buffer
 *  @param offset The offset if required
 */
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
	sprintf(message, "%s(%zu letters)", buffer, len);   // appending received string with its length
	size_of_message = strlen(message);                 // store the length of the stored message
	printk(KERN_INFO "kfetch: Received %zu characters from the user\n", len);
	return len;
}

/** @brief The device release function that is called whenever the device is closed/released by
 *  the userspace program
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_release(struct inode *inodep, struct file *filep){
	printk(KERN_INFO "kfetch: Device successfully closed\n");
	return 0;
}

/** @brief A module must use the module_init() module_exit() macros from linux/init.h, which
 *  identify the initialization function at insertion time and the cleanup function (as
 *  listed above)
 */
module_init(kfetch_init);
module_exit(kfetch_exit);
