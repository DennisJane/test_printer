#include <rtthread.h>
#include "fileDevice.h"

int fileDevice_open(fileDevice_t *dev,char *fname){
	if(dev->fd.flags & DFS_F_OPEN){
		rt_kprintf("fileDevice_open: already opened\r\n");
		return -1;
	}
	if(0 == dfs_file_open(&dev->fd,fname,DFS_O_RDONLY) ){
		return 0;
	}
	rt_kprintf("fileDevice_open: no such file\r\n");
	return -2;
}

int fileDevice_close(fileDevice_t *dev){
	if(dev->fd.flags & DFS_F_OPEN){
		dfs_file_close(&dev->fd);
		dev->fd.flags &=DFS_F_OPEN;
		return 0;
	}else{
		rt_kprintf("fileDecive_close:file not opened\r\n");
		return -1;
	}
}

//libStepGCodeÓÃ½Ó¿Ú
rt_size_t fileDevice_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size){
	fileDevice_t *device=dev->user_data;
	if(device->fd.flags & DFS_F_OPEN){
		return dfs_file_read(&device->fd,buffer,1);
	}
	return 0;
}
rt_size_t fileDevice_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size){
	return size;
}


void fileDevice_init(fileDevice_t *dev,int id)
{
	dev->fd.flags=0;
	rt_device_t device=&(dev->parent);
    device->type        = RT_Device_Class_Char;
    device->rx_indicate = RT_NULL;
    device->tx_complete = RT_NULL;

    device->init        = 0;
    device->open        = 0;
    device->close       = 0;
    device->read        = fileDevice_read;
    device->write       = fileDevice_write;
    device->control     = 0;
	
    device->user_data   = dev;
	
    /* register a character device */
	char buf[10];
	rt_sprintf(buf,"fileDev%d",id);
    rt_device_register(&dev->parent,buf,0);
}

