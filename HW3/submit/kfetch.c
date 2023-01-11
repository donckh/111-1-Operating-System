#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  //hostname
#include <string.h>
#include "kfetch.h"


#define err_quit(msg)			\
	do {				\
		perror(msg);		\
		exit(EXIT_FAILURE);	\
	} while (0)

void kfetch_set_info(int fd, int mask)
{
	int len;
	char buf[KFETCH_BUF_SIZE];
	sprintf(buf, "%d", mask);
	//printf("Writing message to the device [%d] to buf: %s, len: %ld.\n", mask, buf, strlen(buf));
	len = write(fd, buf, strlen(buf));
	//printf("len: %d\n", len);
	//len = write(fd, &mask, sizeof(mask));
	if (len < 0)
		err_quit("write");
}

void kfetch_print_info(int fd)
{
	int len;
	char buf[KFETCH_BUF_SIZE];
	char *arr;
	//printf("Reading from the device...\n");
	
	len = read(fd, buf, KFETCH_BUF_SIZE);
	if (len < 0)
		err_quit("read");

	arr = strrchr(buf, '\n');  //find the end of message
	buf[arr-buf] = '\0';  // add \0 to end the char array

	printf("%s\n", buf);
	len = read(fd, buf, KFETCH_BUF_SIZE);
	
}

void usage(const char *progname)
{
	fprintf(stderr,
		"Usage:\n"
		"\t%s [options]\n",
		progname);
	fprintf(stderr,
		"Options:\n"
		"\t-a  Show all information\n"
		"\t-c  Show CPU model name \n"
		"\t-m  Show memory information\n"
		"\t-n  Show the number of CPU cores\n"
		"\t-p  Show the number of processes\n"
		"\t-r  Show the kernel release information\n"
		"\t-u  Show how long the system has been running\n");
}

int main(int argc, char *argv[])
{
	int fd;
	int opt;
	int mask_info;

	mask_info = -1;
	while ((opt = getopt(argc, argv, "acnmpruh")) != -1) {
		if (mask_info < 0)
			mask_info = 0;
		switch (opt) {
		case 'a': mask_info = KFETCH_FULL_INFO; break;
		case 'c': mask_info |= KFETCH_CPU_MODEL; break;
		case 'm': mask_info |= KFETCH_MEM; break;
		case 'n': mask_info |= KFETCH_NUM_CPUS; break;
		case 'p': mask_info |= KFETCH_NUM_PROCS; break;
		case 'r': mask_info |= KFETCH_RELEASE; break;
		case 'u': mask_info |= KFETCH_UPTIME; break;
		case 'h': usage(*argv); exit(EXIT_SUCCESS);
		case '?':
		fprintf(stderr, "Unknown option: %c\n", optopt);
		usage(*argv);
		exit(EXIT_FAILURE);
		case ':':
		fprintf(stderr, "Missing arg for %c\n", optopt);
		usage(*argv);
		exit(EXIT_FAILURE);
		}
	}

	fd = open(KFETCH_DEV_PATH, O_RDWR);
	if (fd < 0)
		err_quit("open");
	//printf("fd: [%d].\n", fd);
	if (mask_info != -1)
		kfetch_set_info(fd, mask_info);
	kfetch_print_info(fd);  

}
