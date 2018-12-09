#pragma once

#include <rtthread.h>
#include <dfs_file.h>

#define FILE_DEVICE_OPENED 0x00000001

typedef struct{
	struct rt_device parent;	
	struct dfs_fd fd;
}fileDevice_t;

int fileDevice_open(fileDevice_t *dev,char *fname);
int fileDevice_close(fileDevice_t *dev);
void fileDevice_init(fileDevice_t *dev,int id);

