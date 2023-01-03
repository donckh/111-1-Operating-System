#ifndef KFETCH_H
#define KFETCH_H

#define KFETCH_DEV_NAME "kfetch"
#define KFETCH_DEV_PATH "/dev/kfetch"
#define KFETCH_BUF_SIZE 1024

#define KFETCH_NUM_INFO 6

#define KFETCH_RELEASE   (1 << 0)
#define KFETCH_NUM_CPUS  (1 << 1)
#define KFETCH_CPU_MODEL (1 << 2)
#define KFETCH_MEM       (1 << 3)
#define KFETCH_UPTIME    (1 << 4)
#define KFETCH_NUM_PROCS (1 << 5)

#define KFETCH_FULL_INFO ((1 << KFETCH_NUM_INFO) - 1);

#endif
