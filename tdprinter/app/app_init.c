#include "stm32f4xx.h"
#include "bsp/bsp.h"
#include <rtthread.h>

#include <dfs_init.h>
#include <dfs_elm.h>
#include <dfs_fs.h>

#include <finsh.h>
#include <init/components.h>
#include "lib/param/param.h"
#include "romfs/dfs_romfs.h"
#include "app_retarget.h"


static int component_finsh_init(void);
void rt_init_thread_entry(void* parameter)
{
	bsp_init();
	rt_components_board_init();
    rt_components_init();
	//finsh_set_device(FINSH_DEVICE);
}
//-----------INIT-LEVEL-COMPONENT
static int component_finsh_init(void)
{
	retarget_init();
	retarget_finshMode(false);
    return 0;
}
INIT_EXPORT(component_finsh_init, "5.1");
//-----------INIT-LEVEL-FS-----------------------
static int rt_fs_init(void){
	rt_kprintf("Rom File System initing....");
	if (dfs_mount("nullDev", "/", "rom", 0, &romfs_root) == 0){
		rt_kprintf("ok!\r\n");
	}else{ 
		rt_kprintf("failed!\r\n");
	}
	rt_kprintf("SD File System initing....");
	msd_init("sd0", "sddev");
	if (dfs_mount("sd0", "/Micro¿¨", "elm", 0, 0) == 0){
		rt_kprintf("ok!\r\n");
	}else{
		rt_kprintf("failed!\r\n");
	}
	return 0;
}
INIT_EXPORT(rt_fs_init,"5.2");
int32_t param_init(void);
INIT_EXPORT(param_init,"5.3");
//-----------INIT-LVEVL-APP----------------------
static int rt_app_init(void){
	rt_thread_t init_thread;
	void TDPrinter_init(void);
	TDPrinter_init();
	void app_sboard(void *useless);
	init_thread = rt_thread_create("app_sboard",app_sboard, RT_NULL,512,20,1);
	if (init_thread != RT_NULL)rt_thread_startup(init_thread);
	//----usb----
	void usb_thread_entry(void* parameter);
	init_thread = rt_thread_create("app_usb",usb_thread_entry, RT_NULL,1024,21,1);
	if (init_thread != RT_NULL)rt_thread_startup(init_thread);
	usbmsc_register("usbmsc");
	return 0;
}
INIT_EXPORT(rt_app_init,"6.0");

//-------------USELESS---------------------
int rt_application_init()
{
    rt_thread_t init_thread;
    init_thread = rt_thread_create("init",rt_init_thread_entry, RT_NULL,2048, 1, 20);
    if (init_thread != RT_NULL)rt_thread_startup(init_thread);
    return 0;
}

#include <finsh.h>
void reboot(int delay){
	rt_thread_delay(delay);
	NVIC_SystemReset();
}
FINSH_FUNCTION_EXPORT(reboot,reboot());

