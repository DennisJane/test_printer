/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "usbh_usr.h"
#include "usbh_msc_core.h"
#include "usbh_msc_scsi.h"
#include "usbh_msc_bot.h"
#include "bsp/bsp.h"

#include <rtthread.h>

#include <dfs_init.h>
#include <dfs_elm.h>
#include <dfs_fs.h>

//#define LCD_UsrLog rt_kprintf
//#define LCD_ErrLog rt_kprintf
#define LCD_UsrLog(...)
#define LCD_ErrLog(...)

/** @defgroup USBH_USR_Private_Macros
* @{
*/ 
extern USB_OTG_CORE_HANDLE          USB_OTG_Core;
/**
* @}
*/

/** @defgroup USBH_USR_Private_Variables
* @{
*/ 

/*  Points to the DEVICE_PROP structure of current device */
/*  The purpose of this register is to speed up the execution */

USBH_Usr_cb_TypeDef USR_cb =
{
  USBH_USR_Init,
  USBH_USR_DeInit,
  USBH_USR_DeviceAttached,
  USBH_USR_ResetDevice,
  USBH_USR_DeviceDisconnected,
  USBH_USR_OverCurrentDetected,
  USBH_USR_DeviceSpeedDetected,
  USBH_USR_Device_DescAvailable,
  USBH_USR_DeviceAddressAssigned,
  USBH_USR_Configuration_DescAvailable,
  USBH_USR_Manufacturer_String,
  USBH_USR_Product_String,
  USBH_USR_SerialNum_String,
  USBH_USR_EnumerationDone,
  USBH_USR_UserInput,
  USBH_USR_MSC_Application,
  USBH_USR_DeviceNotSupported,
  USBH_USR_UnrecoveredError
    
};

/**
* @}
*/

/** @defgroup USBH_USR_Private_Constants
* @{
*/ 
/*--------------- LCD Messages ---------------*/
const uint8_t MSG_HOST_INIT[]        = "USB: Host Library Initialized\r\n";
const uint8_t MSG_DEV_ATTACHED[]     = "USB:Device Attached \r\n";
const uint8_t MSG_DEV_DISCONNECTED[] = "USB:Device Disconnected\r\n";
const uint8_t MSG_DEV_ENUMERATED[]   = "USB:Enumeration completed \r\n";
const uint8_t MSG_DEV_HIGHSPEED[]    = "USB:High speed device detected\r\n";
const uint8_t MSG_DEV_FULLSPEED[]    = "USB:Full speed device detected\r\n";
const uint8_t MSG_DEV_LOWSPEED[]     = "USB:Low speed device detected\\rn";
const uint8_t MSG_DEV_ERROR[]        = "USB:Device fault \r\n";

const uint8_t MSG_MSC_CLASS[]        = "USB:Mass storage device connected\r\n";
const uint8_t MSG_HID_CLASS[]        = "USB:HID device connected\r\n";
const uint8_t MSG_DISK_SIZE[]        = "USB:Size of the disk in MBytes: \r\n";
const uint8_t MSG_LUN[]              = "USB:LUN Available in the device:\r\n";
const uint8_t MSG_ROOT_CONT[]        = "USB:Exploring disk flash ...\r\n";
const uint8_t MSG_WR_PROTECT[]       = "USB:The disk is write protected\r\n";
const uint8_t MSG_UNREC_ERROR[]      = "USB:UNRECOVERED ERROR STATE\r\n";

/**
* @}
*/


/** @defgroup USBH_USR_Private_FunctionPrototypes
* @{
*/
/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Functions
* @{
*/ 


/**
* @brief  USBH_USR_Init 
*         Displays the message on LCD for host lib initialization
* @param  None
* @retval None
*/
void USBH_USR_Init(void)
{
	static uint8_t startup = 0;  

	if(startup == 0 ){
		startup = 1;
		LCD_UsrLog("USB:Host library started.\r\n"); 
	}
}

/**
* @brief  USBH_USR_DeviceAttached 
*         Displays the message on LCD on device attached
* @param  None
* @retval None
*/
void USBH_USR_DeviceAttached(void)
{
	LCD_UsrLog((void *)MSG_DEV_ATTACHED);
}

USBH_USR_Status USBH_USR_UserInput(void){
	return USBH_USR_RESP_OK;
}
/**
* @brief  USBH_USR_UnrecoveredError
* @param  None
* @retval None
*/
void USBH_USR_UnrecoveredError (void)
{
  
  /* Set default screen color*/ 
  LCD_ErrLog((void *)MSG_UNREC_ERROR); 
}

#ifdef NOT_DEFINED_IN_USBMSC_C_
int usb_msc_mount_ok=0;
/**
* @brief  USBH_USR_MSC_Application 
*         Demo application for mass storage
* @param  None
* @retval Staus
*/
int USBH_USR_MSC_Application(void)
{
	
	if(usb_msc_mount_ok!=0)return -1;
	rt_kprintf("USB:File System mounting....");
	if (dfs_mount("usbmsc", "/U≈Ã", "elm", 0, 0) == 0 ){
		rt_kprintf("ok!\r\n");
		usb_msc_mount_ok=1;
	}else{ 
		rt_kprintf("failed!,err=%d\r\n",rt_get_errno());
		usb_msc_mount_ok=-1;
	}
	return 0;
}

/**
* @brief  USBH_DisconnectEvent
*         Device disconnect event
* @param  None
* @retval Staus
*/
void USBH_USR_DeviceDisconnected (void)
{
	/* Set default screen color*/
	LCD_ErrLog((void *)MSG_DEV_DISCONNECTED);
	if(usb_msc_mount_ok != 1){
		usb_msc_mount_ok=0;
		return;
	}
	usb_msc_mount_ok=0;
	rt_kprintf("USB:File System Unmounting....");
	if (dfs_unmount("/U≈Ã") == 0){
		rt_kprintf("ok!\r\n");
	}else{ 
		rt_kprintf("failed!\r\n");
	}
}
#endif
/**
* @brief  USBH_USR_ResetUSBDevice 
* @param  None
* @retval None
*/
void USBH_USR_ResetDevice(void)
{
  /* callback for USB-Reset */
	LCD_ErrLog("USB:USBH_USR_ResetDevice\r\n"); 
}


/**
* @brief  USBH_USR_DeviceSpeedDetected 
*         Displays the message on LCD for device speed
* @param  Device speed
* @retval None
*/
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
  if(DeviceSpeed == HPRT0_PRTSPD_HIGH_SPEED)
  {
    LCD_UsrLog((void *)MSG_DEV_HIGHSPEED);
  }  
  else if(DeviceSpeed == HPRT0_PRTSPD_FULL_SPEED)
  {
    LCD_UsrLog((void *)MSG_DEV_FULLSPEED);
  }
  else if(DeviceSpeed == HPRT0_PRTSPD_LOW_SPEED)
  {
    LCD_UsrLog((void *)MSG_DEV_LOWSPEED);
  }
  else
  {
    LCD_UsrLog((void *)MSG_DEV_ERROR);
  }
}

/**
* @brief  USBH_USR_Device_DescAvailable 
*         Displays the message on LCD for device descriptor
* @param  device descriptor
* @retval None
*/
void USBH_USR_Device_DescAvailable(void *DeviceDesc)
{ 
  USBH_DevDesc_TypeDef *hs;
  hs = DeviceDesc;  
  hs=hs;
  
  LCD_UsrLog("VID : %04Xh\n" , (uint32_t)(*hs).idVendor); 
  LCD_UsrLog("PID : %04Xh\n" , (uint32_t)(*hs).idProduct); 
}

/**
* @brief  USBH_USR_DeviceAddressAssigned 
*         USB:device is successfully assigned the Address 
* @param  None
* @retval None
*/
void USBH_USR_DeviceAddressAssigned(void)
{
	LCD_UsrLog("USB:USBH_USR_DeviceAddressAssigned\r\n");
}


/**
* @brief  USBH_USR_Conf_Desc 
*         Displays the message on LCD for configuration descriptor
* @param  Configuration descriptor
* @retval None
*/
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
                                          USBH_InterfaceDesc_TypeDef *itfDesc,
                                          USBH_EpDesc_TypeDef *epDesc)
{
  USBH_InterfaceDesc_TypeDef *id;
  
  id = itfDesc;  
  
  if((*id).bInterfaceClass  == 0x08)
  {
    LCD_UsrLog((void *)MSG_MSC_CLASS);
  }
  else if((*id).bInterfaceClass  == 0x03)
  {
    LCD_UsrLog((void *)MSG_HID_CLASS);
  }    
}

/**
* @brief  USBH_USR_Manufacturer_String 
*         Displays the message on LCD for Manufacturer String 
* @param  Manufacturer String 
* @retval None
*/
void USBH_USR_Manufacturer_String(void *ManufacturerString)
{
  LCD_UsrLog("Manufacturer : %s\n", (char *)ManufacturerString);
}

/**
* @brief  USBH_USR_Product_String 
*         Displays the message on LCD for Product String
* @param  Product String
* @retval None
*/
void USBH_USR_Product_String(void *ProductString)
{
  LCD_UsrLog("Product : %s\n", (char *)ProductString);  
}

/**
* @brief  USBH_USR_SerialNum_String 
*         Displays the message on LCD for SerialNum_String 
* @param  SerialNum_String 
* @retval None
*/
void USBH_USR_SerialNum_String(void *SerialNumString)
{
  LCD_UsrLog( "Serial Number : %s\n", (char *)SerialNumString);    
} 



/**
* @brief  EnumerationDone 
*         User response request is displayed to ask application jump to class
* @param  None
* @retval None
*/
void USBH_USR_EnumerationDone(void)
{
  /* Enumeration complete */
  LCD_ErrLog((void *)MSG_DEV_ENUMERATED);
} 


/**
* @brief  USBH_USR_DeviceNotSupported
*         Device is not supported
* @param  None
* @retval None
*/
void USBH_USR_DeviceNotSupported(void)
{
  LCD_ErrLog ("USB:Device not supported."); 
}

/**
* @brief  USBH_USR_OverCurrentDetected
*         Over Current Detected on VBUS
* @param  None
* @retval Staus
*/
void USBH_USR_OverCurrentDetected (void)
{
  LCD_ErrLog ("USB:Overcurrent detected.\r\n");
}

/**
* @brief  USBH_USR_DeInit
*         Deint User state and associated variables
* @param  None
* @retval None
*/
void USBH_USR_DeInit(void)
{
	LCD_ErrLog("USB:USBH_USR_DeInit\r\n");
}


