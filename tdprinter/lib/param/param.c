#include <rtthread.h>
#include <dfs_file.h>
#include "param.h"
#include "utils/myMacro.h"

#define PARAM_VERSION 13
#define PARAM_LENGTH (sizeof(param_t)/4)
	
//INNER
static int32_t param_loadRom(param_t *param);
static int32_t param_saveRom(param_t *param);
static int32_t param_resetRam(param_t *param);
static int32_t CheckValid(param_t * param);

param_t paramRam;
static param_t paramRom;
param_t *param=&paramRam;

// 初始化参数模块。
int32_t param_init(void)
{
	if(param_load()==0){
		return 0;
	}else{
		param_reset();
	}
    return 0;
}

// 计算参数的CRC。
static uint32_t CalcChecksum(param_t *param)
{
    uint32_t *lp=(uint32_t*)param,size=PARAM_LENGTH-1,checksum=0;
	uint32_t i;
	for(i=0;i<size;i++){
		checksum+=lp[i];
	}
	return ~checksum;
}
// 检查参数是否有效。
// 版本不对，CRC校验失败都会导致参数无效。
static int32_t CheckValid(param_t * param)
{
	if(param->checksum!=CalcChecksum(param)){
		rt_kprintf("param.c: CheckValid failed param->checksum=%02X calcChecksum=%02X\r\n",param->checksum,CalcChecksum(param));
		return -1;
	}
    if(param->version != PARAM_VERSION){
		rt_kprintf("param.c:CheckValid failed param->version=%d VERSION=%d failed\r\n",param->version,PARAM_VERSION);
        return -2;
	}
    return 0;
}

param_t	*param_getRamParam(void){
	return &paramRam;
}
const param_t *param_getRomParam(void){
	return &paramRom;
}

// 把RAM中的参数写入到Flash里。
int32_t param_save(void)
{
    paramRam.checksum=CalcChecksum(&paramRam);
    // 检查是否需要写，避免重复写入。
    {
		int32_t same = 1,i;
		for(i=0;i<PARAM_LENGTH-1;i++)
		{
			if( ((uint32_t *)&paramRom)[i] != ((uint32_t *)&paramRam)[i])
			{
				same = 0;
				break;
			}
		}
		if(same)
		{
			rt_kprintf("param.c:param_save ignored\r\n");// 源文件要用UTF8编码。
			return 0;
		}
	}
    paramRom=paramRam;
	if(param_saveRom(&paramRom)!=0){
		rt_kprintf("param.c:param_saveRom failed\r\n");
		return -2;
	}
    //
	rt_kprintf("param.c:param saved\r\n");	// 源文件要用UTF8编码。
    return 0;
}

int32_t param_load(void){
	if(param_loadRom(&paramRom)!=0){
		rt_kprintf("param.c: param_load failed\r\n");
		return -1;
	}
	if(CheckValid(&paramRom) != 0){
		rt_kprintf("param.c:param_load CheckValid failed\r\n");
		return -2;
    }
	rt_kprintf("param.c: param_load successful\r\n");
	paramRam=paramRom;
	return 0;
}

int32_t param_reset(void)
{
    param_resetRam(&paramRam);
	rt_kprintf("param.c:param_reset successful\n");
    return 0;
}


static int32_t param_loadRom(param_t *param){
	struct dfs_fd *fd;
	fd=rt_malloc(sizeof(struct dfs_fd));
	if(fd==0)return -2;
	if(RT_EOK != dfs_file_open(fd,"/Micro卡/.config.bin",DFS_O_RDONLY|DFS_O_BINARY)){
		rt_kprintf("param.c:param_loadRom dfs_file_open failed\r\n");
		return -1;
	}
	dfs_file_read(fd,param,sizeof(param_t));
	dfs_file_close(fd);
	rt_free(fd);
	return 0;
}
static int32_t param_saveRom(param_t *param){
	struct dfs_fd *fd;
	fd=rt_malloc(sizeof(struct dfs_fd));
	if(fd==0)return -2;
	if(RT_EOK != dfs_file_open(fd,"/Micro卡/.config.bin",DFS_O_WRONLY|DFS_O_CREAT|DFS_O_TRUNC|DFS_O_BINARY)){
		rt_kprintf("param.c:param_saveRom dfs_file_open failed\r\n");
		return -1;
	}
	dfs_file_write(fd,param,sizeof(param_t));
	dfs_file_close(fd);
	rt_free(fd);
	return 0;
}



static int32_t param_resetRam(param_t *param){
	// 全部清0，避免不小心没初始化。
    for(int i=0;i<PARAM_LENGTH;i++)
        ((uint8_t *)param)[i] = 0;
    //
    param->version = PARAM_VERSION;
    //
	SET4(param->positionOffset,0,0,0,0);
	
	//maxAccelSpeed[4] = 1000,600,600,50
	//maxSpeed[4]= 500,280,50,15
	
	#ifdef TDPRINTER_B
	SET4(param->mm2step,5.0f,5.0f,6.0f,640.0f);
    SET4(param->microStep,16,16,16,1);
	#else
    #pragma warning "other tdprinter type need calibrate"
    SET4(param->mm2step,1.0f,1.0f,1.0f,1.0f);
    SET4(param->microStep,16,16,16,16);
    #endif
	SET4(param->maxSpeed,200.0f,200.0f,50.0f,10.0f);		//XYEZ
	SET4(param->maxSpeedDelta,25.0f,25.0f,100.0f,0.2f);
	SET4(param->defaultEntrySpeed,1.0f,1.0f,10.0f,3.0f);
	SET4(param->defaultAccelSpeed,1000.0f,1000.0f,10000.0f,200.0f);
	SET4(param->maxAccelSpeed,3000.0f,3000.0f,5000.0f,200.0f);
    param->advanceK=0.0f;
	param->advanceDeprime=0.0f;
	//END
    param->checksum=CalcChecksum(param);
    return 0;
}
