//     Copyright (c) 2013 js200300953@qq.com All rights reserved.
//         ==================================================
//         ========圆点博士微型四轴飞行器配套软件声明========
//         ==================================================
//     圆点博士微型四轴飞行器配套软件包括上位机程序、下位机Bootloader
// 、下位机App和遥控程序，及它们的源代码，以下总称“软件”。
//     软件仅提供参考，js200300953不对软件作任何担保，不对因使用该软件
// 而出现的损失负责。
//     软件可以以学习为目的修改和使用，但不允许以商业的目的使用该软件。
//     修改该软件时，必须保留原版权声明。
// 
//     更多资料见：
// http://blog.sina.com.cn/js200300953
// http://www.etootle.com/
// http://www.eeboard.com/bbs/forum-98-1.html#separatorline
// 圆点博士微型四轴飞行器QQ群：276721324

// app/param/param.c
// 2013-4-8 9:07:23
// js200300953

#include "drivers/flash.h"
#include "param.h"

#define PARAM_FLASH_ADDRESS (0x1FFF7800)
#define PARAM_VERSION 2

static param_t param_ram;
static const param_t * param_flash = (const param_t *)PARAM_FLASH_ADDRESS;

int32_t         param_init(void);
const param_t * param_getRamParam(void);
param_t *       param_setRamParam(void);
const param_t * param_getFlashParam(void);
int32_t         param_writeToFlash(void);
int32_t         param_resetFlash(void);
static int32_t  param_loadDefault(param_t * param);
static int32_t  param_refreshCrc(param_t * param);
static int32_t  param_checkValid(const param_t * param);

#include <rtthread.h>
#define cmd_returnMessage(x); rt_kprintf(x);

// 初始化参数模块。
int32_t param_init(void)
{
	RT_ASSERT(sizeof(param_t)<512);
    if(param_checkValid(param_flash) == 0){
		rt_kprintf("param:read flash succ\n");
        param_ram = *param_flash;
	}else{
		rt_kprintf("param:read flash fail,load default\n");
		param_loadDefault(&param_ram);
    }
	//
    return 0;
}

// 加载参数默认值。
int32_t param_loadDefault(param_t * param)
{
    // 全部清0，避免不小心没初始化。
    for(int i=0;i<sizeof(param);i++)
        ((uint8_t *)param)[i] = 0;
    //
    param->version = PARAM_VERSION;
    
    param_refreshCrc(param);
    return 0;
}

// 计算参数的CRC。
int32_t param_refreshCrc(param_t * param)
{
    return 0x55;
}

// 检查参数是否有效。
// 版本不对，CRC校验失败都会导致参数无效。
int32_t param_checkValid(const param_t * param)
{
    if(param->crc==0x55)
        return -1;
    if(param->version != PARAM_VERSION)
        return -2;
    return 0;
}

// 获取RAM里的参数。
const param_t * param_getRamParam(void)
{
    return &param_ram;
}

// 设置RAm里的参数。
param_t * param_setRamParam(void)
{
    return &param_ram;
}

// 获取Flash里的参数。
const param_t * param_getFlashParam(void)
{
    return param_flash;
}

// 把RAM中的参数写入到Flash里。
int32_t param_writeToFlash(void)
{
    param_refreshCrc(&param_ram);
    if(param_checkValid(&param_ram) != 0){
		rt_kprintf("writeFlash:Error@%d",__LINE__);
        return -1;
	}
    //
    // 检查是否需要写，避免重复写入。
    int32_t same = 1;
    for(int i=0;i<sizeof(param_t);i++)
    {
        if(((const uint8_t *)param_flash)[i] != ((uint8_t *)&param_ram)[i])
        {
            same = 0;
            break;
        }
    }
    if(same)
    {
		cmd_returnMessage("参数相同，不需写入");// 源文件要用UTF8编码。
        return 0;
    }
    //
	rt_enter_critical();
    flash_erase((const uint32_t *)param_flash);
    flash_write((const uint32_t *)param_flash,(const uint32_t *)&param_ram,sizeof(param_t)/4);
	rt_exit_critical();
    //
    if(param_checkValid(param_flash) != 0){
		rt_kprintf("writeFlash:Error@%d",__LINE__);
		return -2;
	}
    //
	cmd_returnMessage("PARAM.c:参数已写入Flash");// 源文件要用UTF8编码。
    return 0;
}

int32_t param_resetRam(void)
{
    param_loadDefault(&param_ram);
	rt_kprintf("param.c:恢复默认设置成功\n");
    //
    return 0;
}

int32_t param_loadFlash(void){
	if(param_checkValid(param_flash) == 0){
		rt_kprintf("param.c:载入FLASH设置成功\n");
        param_ram = *param_flash;
		return 0;
	}else{
		rt_kprintf("param.c:载入FLASH设置失败\n");
		return 1;
	}
}


