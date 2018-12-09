#include "corePrinter.h"
#include "app/app_subboard.h"
#include <math.h>

static void logMovedDistance(corePrinter_t *c);

void corePrinter_construct(corePrinter_t *c){
	stepGCode_construct(&c->gcodeDevice);
	c->printFileComplete=100;
}

void corePrinter_init(corePrinter_t *c,rt_device_t defaultDev){
	stepGCode_init(&c->gcodeDevice);
	fileDevice_init(&c->fileDevice,0);
	c->defaultDevice=defaultDev;
	c->fileName[0]=0;
	stepGCode_start(&c->gcodeDevice,0,c->defaultDevice);
	
	logMovedDistance(c);
	{
		void corePrinter_thread(void *param);
		c->thread=rt_thread_create("CPThread",corePrinter_thread,c,THREAD_CORE_PRINTER_STACK_SIZE,THREAD_CORE_PRINTER_PRIORITY,1);
		rt_thread_startup(c->thread);
	}
}

void corePrinter_start(corePrinter_t *c,char *filename){
	fileDevice_close(&c->fileDevice);
	stepGCode_stop(&c->gcodeDevice);
	fileDevice_open(&c->fileDevice,filename);
	stepGCode_start(&c->gcodeDevice,1,&c->fileDevice.parent);
	c->printFileComplete=0;
	rt_strncpy(c->fileName,filename,25);
	c->timeUsed=0;
	c->timeBase=rt_tick_get();
	c->gcodeDevice.stepPos.distanceMoved=0;
}


void corePrinter_stop(corePrinter_t *c){
	fileDevice_close(&c->fileDevice);
	stepGCode_stop(&c->gcodeDevice);
	c->printFileComplete=100;
	sboard_setLed(0);
	sboard_setTemper(0);
	sboard_setFan(0);
	{
		logMovedDistance(c);
	}
	stepGCode_start(&c->gcodeDevice,0,c->defaultDevice);
}
void corePrinter_thread(void *param){
	corePrinter_t *c=param;
	while(1){
		if(c->printFileComplete<100){
			sboard_setLed(1);
			if(c->fileDevice.fd.size==0){
				c->printFileComplete=100;
			}else{
				float percentage=(c->fileDevice.fd.pos)/(c->fileDevice.fd.size);
				c->printFileComplete=percentage*100;    
			}
			if(c->printFileComplete==100){
				stepBlock_waitForIdle(&c->gcodeDevice.stepPos.planner.stepBlock,RT_WAITING_FOREVER);
				corePrinter_stop(c);
			}
			c->timeUsed = rt_tick_get() - c->timeBase;
		}
		rt_thread_delay(10);
	}
}

static void logMovedDistance(corePrinter_t *c){
	struct dfs_fd *fd;
	fd=rt_malloc(sizeof(struct dfs_fd));
	if(fd==0)return;
	if(RT_EOK == dfs_file_open(fd,"/Micro¿¨/.distance.log",DFS_O_RDWR|DFS_O_BINARY)){
		dfs_file_read(fd,&c->totalDistance,sizeof(c->totalDistance));
		if(isnan(c->totalDistance)){
			c->totalDistance=0.0;
		}
	}else if(RT_EOK != dfs_file_open(fd,"/Micro¿¨/.distance.log",DFS_O_CREAT|DFS_O_RDWR|DFS_O_BINARY)){
		return;
	}
	c->totalDistance +=c->gcodeDevice.stepPos.distanceMoved;
	dfs_file_lseek(fd,0);
	dfs_file_write(fd,&c->totalDistance,sizeof(c->totalDistance));
	dfs_file_close(fd);
	rt_free(fd);
}
