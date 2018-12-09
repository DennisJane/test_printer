#include "usbmsc.h"
#include "usb_conf.h"
#include "diskio.h"
#include "usbh_msc_core.h"
#include <dfs_init.h>
#include <dfs_elm.h>
#include <dfs_fs.h>

static struct rt_device usbmsc;

typedef enum{
	USB_APP_NOT_PLUGIN=0,
	USB_APP_MOUNT_FAILED,
	USB_APP_MOUNT_SUCC,
}USB_APP_STATUS;
typedef enum{
	USB_REQ_NONE=0,
	USB_REQ_READING,
	USB_REQ_WRITING,
}USB_APP_REQUEST;
static USB_APP_STATUS appStatus;
static USB_APP_REQUEST reqStatus;
typedef struct{
	int status;
	int pos,size;
	uint8_t *buffer;
}USB_APP_PARAM;
static USB_APP_PARAM ioParam;

extern USB_OTG_CORE_HANDLE          USB_OTG_Core;
extern USBH_HOST                     USB_Host;

static rt_size_t rt_usb_read(struct rt_device *dev,
                                rt_off_t          pos,
                                void             *buffer,
                                rt_size_t         size)
{
	if(appStatus!=USB_APP_MOUNT_SUCC){
		BYTE status = USBH_MSC_OK;
		if(HCD_IsDeviceConnected(&USB_OTG_Core)){  
			do{
				status = USBH_MSC_Read10(&USB_OTG_Core,(BYTE*)buffer,pos,512*size);
				USBH_MSC_HandleBOTXfer(&USB_OTG_Core, &USB_Host);
				if(!HCD_IsDeviceConnected(&USB_OTG_Core)){
					return RES_ERROR;
				}
			}
			while(status == USBH_MSC_BUSY );
		}

		if(status == USBH_MSC_OK){
			return size;
		}else{
			return 0;
		}
	}
	if(reqStatus!=USB_REQ_NONE){
		return 0;
	}
	ioParam.pos=pos;
	ioParam.buffer=buffer;
	ioParam.size=size;
	reqStatus=USB_REQ_READING;
	while(1){
		if(reqStatus==USB_REQ_NONE){
			break;
		}else{
			rt_thread_delay(1);
		}
	}
	return size;
}

static rt_size_t rt_usb_write(struct rt_device *dev,
                                 rt_off_t          pos,
                                 const void       *buffer,
                                 rt_size_t         size)
{
    if(appStatus!=USB_APP_MOUNT_SUCC){
		BYTE status = USBH_MSC_OK;
		if(HCD_IsDeviceConnected(&USB_OTG_Core)){  
			do{
				status = USBH_MSC_Write10(&USB_OTG_Core,(BYTE*)buffer,pos,512*size);
				USBH_MSC_HandleBOTXfer(&USB_OTG_Core, &USB_Host);
				if(!HCD_IsDeviceConnected(&USB_OTG_Core)){
					return RES_ERROR;
				}
			}
			while(status == USBH_MSC_BUSY );
		}

		if(status == USBH_MSC_OK){
			return size;
		}else{
			return 0;
		}
	}
	if(reqStatus!=USB_REQ_NONE){
		return 0;
	}
	ioParam.pos=pos;
	ioParam.buffer=(void*)buffer;
	ioParam.size=size;
	reqStatus=USB_REQ_WRITING;
	while(1){
		if(reqStatus==USB_REQ_NONE){
			break;
		}else{
			rt_thread_delay(1);
		}
	}
	return size;
}

static rt_err_t rt_usb_control(struct rt_device *dev,
                                  rt_uint8_t        cmd,
                                  void             *args)
{
    RT_ASSERT(dev != RT_NULL);
	
    if (cmd == RT_DEVICE_CTRL_BLK_GETGEOME)
    {
        struct rt_device_blk_geometry *geometry;

        geometry = (struct rt_device_blk_geometry *)args;
        if (geometry == RT_NULL) return -RT_ERROR;

        geometry->bytes_per_sector = 512;
        geometry->block_size = 512;
        geometry->sector_count = USBH_MSC_Param.MSCapacity;
    }
	
    return RT_EOK;
}


rt_err_t usbmsc_register(const char *name){
    rt_device_t device=&usbmsc;

    device->type        = RT_Device_Class_Block;
    device->rx_indicate = RT_NULL;
    device->tx_complete = RT_NULL;

    device->init        = 0;
    device->open        = 0;
    device->close       = 0;
    device->read        = rt_usb_read;
    device->write       = rt_usb_write;
    device->control     = rt_usb_control;
    device->user_data   = 0;

    return rt_device_register(device, name, RT_DEVICE_FLAG_RDWR|RT_DEVICE_FLAG_REMOVABLE);
}


int USBH_USR_MSC_Application(void)
{
	switch(appStatus){
		case USB_APP_NOT_PLUGIN:
			rt_kprintf("USB:File System mounting....");
			if (dfs_mount("usbmsc", "/usb", "elm", 0, 0) == 0 ){
				rt_kprintf("ok!\r\n");
				appStatus=USB_APP_MOUNT_SUCC;
			}else{ 
				rt_kprintf("failed!,err=%d\r\n",rt_get_errno());
				appStatus=USB_APP_MOUNT_FAILED;
			}
			break;
		case USB_APP_MOUNT_SUCC:
			if(reqStatus==USB_REQ_NONE){
				break;
			}
			ioParam.status = USBH_MSC_OK;
			if(HCD_IsDeviceConnected(&USB_OTG_Core)){
				do{
					if(reqStatus==USB_REQ_READING){
						ioParam.status = USBH_MSC_Read10(&USB_OTG_Core,(BYTE*)ioParam.buffer,ioParam.pos,512*ioParam.size);
					}else{
						ioParam.status = USBH_MSC_Write10(&USB_OTG_Core,(BYTE*)ioParam.buffer,ioParam.pos,512*ioParam.size);
					}
					USBH_MSC_HandleBOTXfer(&USB_OTG_Core, &USB_Host);
					if(!HCD_IsDeviceConnected(&USB_OTG_Core)){
						ioParam.status = RES_ERROR;
						break;
					}
				}
				while(ioParam.status == USBH_MSC_BUSY );
			}
			if(ioParam.status != USBH_MSC_OK){
				ioParam.size=0;
			}
			reqStatus = USB_REQ_NONE;
			break;
		default:
			break;
	}
	return 0;
}

void USBH_USR_DeviceDisconnected (void)
{
	if(appStatus!=USB_APP_NOT_PLUGIN){
		rt_kprintf("USB:File System Unmounting....");
		if (dfs_unmount("/usb") == 0){
			rt_kprintf("ok!\r\n");
		}else{ 
			rt_kprintf("failed!\r\n");
		}
		appStatus=USB_APP_NOT_PLUGIN;
		reqStatus=USB_REQ_NONE;
	}
}

