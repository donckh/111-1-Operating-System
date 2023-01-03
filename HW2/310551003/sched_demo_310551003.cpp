#include <iostream>
#include <string.h>
#include <thread>
#include <chrono>
#include <stdio.h>
#include <sched.h> // for sched_getcpu()
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <mutex>
#include <assert.h>
#include <errno.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/time.h>
using namespace std;

extern char* optarg;
extern int optind;
extern int opterr;
extern int optopt;

pthread_barrier_t barrier;	//create barrier variable;

typedef struct {
	int thread_id;
	double time;
	int sched_policy;
	int sched_priority;
} thread_info_t;

vector <int> plc_sep(char *plc_total, int num_thread){
	vector <int> plc_vec;
	char *plc = strtok(plc_total, ",");

	if(plc) {

			if (!strcmp(plc, "FIFO")) { //strcmp return 0 if equal
				plc_vec.push_back(1);
			}
			else{
				plc_vec.push_back(0);
		}
	}

	for (int i=1; i<num_thread; i++){
		plc = strtok(NULL, ",");
		if(plc) {
			if (!strcmp(plc, "FIFO")) { //strcmp return 0 if equal
				plc_vec.push_back(1);
			}
			else{
				plc_vec.push_back(0);
			}
		}
	}
	return plc_vec;
}


vector <int> pri_sep(char *pri_total, int num_thread){
	vector <int> pri_vec;
	char *pri = strtok(pri_total, ",");
	if(pri) {
		pri_vec.push_back(atoi(pri));
	}
	for (int i=1; i<num_thread; i++){
		pri = strtok(NULL, ",");
		if(pri) {
			pri_vec.push_back(atoi(pri));
		}
	}
	return pri_vec;
}


void* thread_func(void *arg) {  //int t, 
	thread_info_t tmp = *(thread_info_t *)arg;
	int mid = tmp.thread_id;  //dereference void *arg point to int *arg
	double time = tmp.time;
	int policy = tmp.sched_policy;
	int priority = tmp.sched_priority;
	int s = 0;
	
	if (policy==1) {
		struct sched_param param;
		param.sched_priority = priority;
		s = sched_setscheduler(0, SCHED_FIFO, &param);
	}
	pthread_barrier_wait(&barrier);  //wait for all thread and start at once
	for (int it = 0; it < 3; it++) {
		printf("Thread %d is running\n", mid);
		// busy 1 second, time is 1second ratio
		struct timeval start;
		struct timeval end;	
		unsigned long diff;
		unsigned long target=1000000*time;

		gettimeofday(&start,NULL);
		do{
			gettimeofday(&end,NULL);
			diff = 1000000 * (end.tv_sec-start.tv_sec) + end.tv_usec-start.tv_usec;
		}
		while(diff<target);
	}
	return (void *) 0;
}

int main(int argc, char *argv[]) {  //argc: index, argv: value
	int scheduler = 0;	// default scheduler
	thread_info_t thread_para;
	int opt, num_thread;
	double time;
	char *plc_total=0, *pri_total;
	vector <int> plc_vec;
	vector <int> pri_vec;
	while ((opt = getopt(argc, argv, "n:t:s:p:")) != -1)
	{
		switch(opt)
		{
		case 'n':
		num_thread = atoi(optarg);  //convert char array to int
		break;
		case 't':
		time = atof(optarg);
		break;
		case 's':  //policy
		plc_total = optarg;
		break;
		case 'p':  //priority
		pri_total = optarg;
		break;
		case ':':
		puts("oops");
		break;
		case '?':
		puts("wrong command");
		break;
		}
	}

	//seperate char array by each thread policy and save to vector.
	plc_vec = plc_sep(plc_total, num_thread);
	
	//seperate char array by each thread priority and save to vector.
	pri_vec = pri_sep(pri_total, num_thread);

	pthread_t tid[num_thread];  //create thread
	
	//set CPU affinity
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(0, &mask);  // u
	sched_setaffinity(0, sizeof(mask), &mask);  

	pthread_barrier_init(&barrier, NULL, num_thread);  //init barrier
	
	//set attribute to threads
	for (int i = num_thread; i >0; i--) {
		thread_para.thread_id=i-1;
		thread_para.time=time;
		thread_para.sched_policy=plc_vec[i-1];
		thread_para.sched_priority=pri_vec[i-1];
		int err = pthread_create(&tid[i], NULL, thread_func, &thread_para);
		assert(err == 0);
		sleep(1);
	}
	//wait for all threads finish if any remaining
	for (int i = num_thread; i >0; i--)
		pthread_join(tid[i], NULL);
	return 0;
}
