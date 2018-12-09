#include <rtthread.h>
#include <string.h>
#include "hmi_driver.h"

static rt_device_t uart;

void HMI_DriverInit(rt_device_t device){
	uart=device;
}
void SendChar(uint8_t d){
	uint8_t buffer[1]={d};
	rt_device_write(uart,0,buffer,1);
}
void SendStrings(char *d){
	rt_size_t size=strlen(d);
	rt_device_write(uart,0,d,size);
}

/****************************************************************************
* ��    �ƣ� SendBeginCMD()
* ��    �ܣ� ����֡ͷ����
* ��ڲ����� ��      
* ���ڲ����� ��
****************************************************************************/
void SendBeginCMD()
{
 SendChar(0xEE);
}

/****************************************************************************
* ��    �ƣ� SendEndCmd()
* ��    �ܣ� ����֡β����
* ��ڲ����� ��      
* ���ڲ����� ��
****************************************************************************/
void SendEndCmd()
{
 SendChar(0xFF);
 SendChar(0xFC);
 SendChar(0xFF);
 SendChar(0xFF);
}

/****************************************************************************
* ��    �ƣ� SetHandShake()
* ��    �ܣ� ����
* ��ڲ����� ��      
* ���ڲ����� ��
****************************************************************************/
void SetHandShake(void)
{
  SendBeginCMD();
  SendChar(0x00);
  SendEndCmd();
}


/****************************************************************************
* ��    �ƣ� SetFcolor()
* ��    �ܣ� ����ǰ��ɫ
* ��ڲ����� fcolor  ǰ��ɫRGB����   
* ���ڲ����� ��
****************************************************************************/
void SetFcolor(uint16_t fcolor)
{
  SendBeginCMD();
  SendChar(0x41);
  SendChar((fcolor>>8)&0xff);
  SendChar(fcolor&0xff);
  SendEndCmd();
}

/****************************************************************************
* ��    �ƣ� SetBcolor()
* ��    �ܣ� ���ñ���ɫ
* ��ڲ����� bcolor  ����ɫRGB����   
* ���ڲ����� ��
****************************************************************************/
void SetBcolor(uint16_t bcolor)
{
  SendBeginCMD();
  SendChar(0x42);
  SendChar((bcolor>>8)&0xff);
  SendChar(bcolor&0xff);
  SendEndCmd();
}

/****************************************************************************
* ��    �ƣ� ColorPicker()
* ��    �ܣ� ȡ��Ļ������ǰ��ɫ/����ɫ
* ��ڲ����� mode ȡ��ǰ��ʾ��Ļ(X,Y)������ɫ��Ϊǰ��/����ɫ
             x       �Ե�Ϊ��λ��X������ֵ
             y       �Ե�Ϊ��λ��Y������ֵ
* ���ڲ����� ��
****************************************************************************/
void ColorPicker(uint8_t mode, uint16_t x,uint16_t y)
{
  SendBeginCMD();
  SendChar(0xa3);
  SendChar(mode);
  SendChar((x>>8)&0xff);
  SendChar(x&0xff);
  SendChar((y>>8)&0xff);
  SendChar(y&0xff);
  SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� GUI_CleanScreen()
* ��    �ܣ� ����
* ��ڲ����� ��  
* ���ڲ����� ��
****************************************************************************/
void GUI_CleanScreen()
{
  SendBeginCMD();
  SendChar(0x01);
  SendEndCmd();
}

/****************************************************************************
* ��    �ƣ� SetTextSpace()
* ��    �ܣ� �����������м��
* ��ڲ����� x_w     �Ե�Ϊ��λ���м�࣬ȡֵ��Χ00~1F
             y_w     �Ե�Ϊ��λ���м�࣬ȡֵ��Χ00~1F	
* ���ڲ����� ��
****************************************************************************/
void SetTextSpace(uint8_t x_w, uint8_t y_w)
{
  SendBeginCMD();
  SendChar(0x43);
  SendChar(x_w);
  SendChar(y_w);
  SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� DisText_Region()
* ��    �ܣ� �޶��ı���ʾ��Χ
* ��ڲ����� enable   �����ı�ʹ��
             length   �ı����޶��ĳ���
	     width    �ı����޶��ĸ߶�
* ���ڲ����� ��
****************************************************************************/
void SetFont_Region(uint8_t enable,uint16_t length,uint16_t width )
{
  SendBeginCMD();
  SendChar(0x45);
  SendChar(enable);
  SendChar((length>>8)&0xff);
  SendChar(length&0xff);
  SendChar((width>>8)&0xff);
  SendChar(width&0xff);
  SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� SetFilterColor()
* ��    �ܣ� ���ù�����ɫ
* ��ڲ����� fillcolor_dwon   �˳���ɫ������ֵ
             fillcolor_up     �˳���ɫ������ֵ	
* ���ڲ����� ��
****************************************************************************/
void SetFilterColor(uint16_t fillcolor_dwon, uint16_t fillcolor_up)
{
  SendBeginCMD();
  SendChar(0x44);
  SendChar((fillcolor_dwon>>8)&0xff);
  SendChar(fillcolor_dwon&0xff);
  SendChar((fillcolor_up>>8)&0xff);
  SendChar(fillcolor_up&0xff);
  SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� DisText()
* ��    �ܣ� �ı���ʾ
* ��ڲ����� x         �Ե�Ϊ��λ��X������ֵ
             y         �Ե�Ϊ��λ��Y������ֵ
   	         back      ����ɫʹ��
	         font      �ֿ����
             strings   д����ַ���
* ���ڲ����� ��
****************************************************************************/
void DisText(uint16_t x, uint16_t y,uint8_t back,uint8_t font,char *strings )
{
  SendBeginCMD();
  SendChar(0x20);
  SendChar((x>>8)&0xff);
  SendChar(x&0xff);
  SendChar((y>>8)&0xff);
  SendChar(y&0xff);
  SendChar(back);
  SendChar(font);
  SendStrings(strings);
  SendEndCmd();
}

/****************************************************************************
* ��    �ƣ� DisCursor()
* ��    �ܣ� �����ʾ
* ��ڲ����� enable
             x        �Ե�Ϊ��λ��X������ֵ
             y        �Ե�Ϊ��λ��Y������ֵ
             length   ��곤��
	         width    �����
* ���ڲ����� ��
****************************************************************************/
void DisCursor(uint8_t enable,uint16_t x, uint16_t y,uint8_t length,uint8_t width )
{
  SendBeginCMD();
  SendChar(0x21);
  SendChar(enable);
  SendChar((x>>8)&0xff);
  SendChar(x&0xff);
  SendChar((y>>8)&0xff);
  SendChar(y&0xff);
  SendChar(length&0xff);
  SendChar(width&0xff);
  SendEndCmd();
}

/****************************************************************************
* ��    �ƣ� DisFull_Image()
* ��    �ܣ� ȫ������ͼ��ʾ
* ��ڲ����� image_id   ͼƬID���
             masken     ����ɫʹ��
* ���ڲ����� ��
****************************************************************************/
void DisFull_Image(uint16_t image_id,uint8_t masken)
{
  SendBeginCMD();
  SendChar(0x31);
  SendChar((image_id>>8)&0xff);
  SendChar(image_id&0xff); 
  SendChar(masken);
  SendEndCmd();
}

/****************************************************************************
* ��    �ƣ� DisArea_Image()
* ��    �ܣ� ����ͼƬ��ʾ
* ��ڲ����� x          �Ե�Ϊ��λ��X������ֵ
             y          �Ե�Ϊ��λ��Y������ֵ 
             image_id   ͼƬID���
             masken     ����ɫʹ��
* ���ڲ����� ��
****************************************************************************/
void DisArea_Image(uint16_t x,uint16_t y,uint16_t image_id,uint8_t masken)
{
  SendBeginCMD();
  SendChar(0x32);
  SendChar((x>>8)&0xff);
  SendChar(x&0xff);
  SendChar((y>>8)&0xff);
  SendChar(y&0xff);
  SendChar((image_id>>8)&0xff);
  SendChar(image_id&0xff);
  SendChar(masken);
  SendEndCmd();
}

/****************************************************************************
* ��    �ƣ� DisCut_Image()
* ��    �ܣ� ͼƬ����
* ��ڲ����� x          �Ե�Ϊ��λ��X������ֵ
             y          �Ե�Ϊ��λ��Y������ֵ 
             image_id   ͼƬ���
             image_x    ͼƬ�ڲ�X����
             image_y    ͼƬ�ڲ�Y����
             image_l    ���г���
             image_w    ���п��
             masken     ����ɫʹ��
* ���ڲ����� ��
****************************************************************************/
void DisCut_Image(uint16_t x,uint16_t y,uint16_t image_id,uint16_t image_x,uint16_t image_y,
                   uint16_t image_l, uint16_t image_w,uint8_t masken)
{
  SendBeginCMD();
  SendChar(0x33);
  SendChar((x>>8)&0xff);
  SendChar(x&0xff);
  SendChar((y>>8)&0xff);
  SendChar(y&0xff);
  SendChar((image_id>>8)&0xff);
  SendChar(image_id&0xff);
  SendChar((image_x>>8)&0xff);
  SendChar(image_x&0xff);
  SendChar((image_y>>8)&0xff);
  SendChar(image_y&0xff);
  SendChar((image_l>>8)&0xff);
  SendChar(image_l&0xff);
  SendChar((image_w>>8)&0xff);
  SendChar(image_w&0xff);
  SendChar(masken);
  SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� DisFlashImgae()
* ��    �ܣ� ������ʾ
* ��ڲ����� x               �Ե�Ϊ��λ��X������ֵ
             y               �Ե�Ϊ��λ��Y������ֵ 
             flashimage_id   GIF����ID���     
    	     enable          �ر�/������������
	         playnum         �������Ŵ��� 0:һֱ���ţ�1~FF:���Ŵ���
* ���ڲ����� ��
****************************************************************************/
void DisFlashImage(uint16_t x,uint16_t y,uint16_t flashimage_id,uint8_t enable,uint8_t playnum)
{
  SendBeginCMD();
  SendChar(0x80);
  SendChar((x>>8)&0xff);
  SendChar(x&0xff);
  SendChar((y>>8)&0xff);
  SendChar(y&0xff);
  SendChar((flashimage_id>>8)&0xff);
  SendChar(flashimage_id&0xff);
  SendChar(enable);
  SendChar(playnum);
  SendEndCmd();
}

/****************************************************************************
* ��    �ƣ� GUI_Dot()
* ��    �ܣ� ����
* ��ڲ����� x       �Ե�Ϊ��λ��X������ֵ
             y       �Ե�Ϊ��λ��Y������ֵ           
* ���ڲ����� ��
****************************************************************************/
void GUI_Dot(uint16_t x,uint16_t y)
{
  SendBeginCMD();
  SendChar(0x50);
  SendChar((x>>8)&0xff);
  SendChar(x&0xff);
  SendChar((y>>8)&0xff);
  SendChar(y&0xff);
  SendEndCmd();
}

/****************************************************************************
* ��    �ƣ� GUI_Line()
* ��    �ܣ� ����
* ��ڲ����� x0      �Ե�Ϊ��λ��X������ֵ
             y0      �Ե�Ϊ��λ��Y������ֵ    
             x1      �Ե�Ϊ��λ��X������ֵ
             y1      �Ե�Ϊ��λ��Y������ֵ  
* ���ڲ����� ��
****************************************************************************/
void GUI_Line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  SendBeginCMD();
  SendChar(0x51);
  SendChar((x0>>8)&0xff);
  SendChar(x0&0xff);
  SendChar((y0>>8)&0xff);
  SendChar(y0&0xff);
  SendChar((x1>>8)&0xff);
  SendChar(x1&0xff);
  SendChar((y1>>8)&0xff);
  SendChar(y1&0xff);
  SendEndCmd();
}

/****************************************************************************
* ��    �ƣ� GUI_ConDots()
* ��    �ܣ� ��������,��ָ����������Զ���������
* ��ڲ����� mode     0:��ǰ��ɫ��� 1���ñ���ɫ���    
	         pDot     ָ����ָ��   
			 num      ���ݵĸ���  
* ���ڲ����� ��
****************************************************************************/
void GUI_ConDots(uint8_t mode,char *pDot,uint16_t num)
{
 uint16_t i;
  SendBeginCMD();
  SendChar(0x63);
  SendChar(mode);
  for(i=0;i<num;i++)
  {
   SendChar((*pDot>>8)&0xff);
   SendChar(*pDot++&0xff); 
  }
  SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� GUI_Circle()
* ��    �ܣ� ������Բ
* ��ڲ����� x               �Ե�Ϊ��λ��X������ֵ
             y               �Ե�Ϊ��λ��Y������ֵ    
             r                ����Բ�İ뾶   
* ���ڲ����� ��
****************************************************************************/
void GUI_Circle(uint16_t x, uint16_t y, uint16_t r)
{
  SendBeginCMD();
  SendChar(0x52);
  SendChar((x>>8)&0xff);
  SendChar(x&0xff);
  SendChar((y>>8)&0xff);
  SendChar(y&0xff);
  SendChar((r>>8)&0xff);
  SendChar(r&0xff);
  SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� GUI_CircleFill()
* ��    �ܣ� ��ʵ��Բ
* ��ڲ����� x               �Ե�Ϊ��λ��X������ֵ
             y               �Ե�Ϊ��λ��Y������ֵ    
             r                ʵ��Բ�İ뾶   
* ���ڲ����� ��
****************************************************************************/
void GUI_CircleFill(uint16_t x, uint16_t y, uint16_t r)
{
  SendBeginCMD();
  SendChar(0x53);
  SendChar((x>>8)&0xff);
  SendChar(x&0xff);
  SendChar((y>>8)&0xff);
  SendChar(y&0xff);
  SendChar((r>>8)&0xff);
  SendChar(r&0xff);
  SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� GUI_Arc()
* ��    �ܣ� ��Բ��
* ��ڲ����� x               �Ե�Ϊ��λ��X������ֵ
             y               �Ե�Ϊ��λ��Y������ֵ    
             r               Բ�İ뾶  
 	         sa              ��ʼ�Ƕ�
	         ea              �����Ƕ�
* ���ڲ����� ��
****************************************************************************/
void GUI_Arc(uint16_t x,uint16_t y, uint16_t r,uint16_t sa, uint16_t ea)
{
  SendBeginCMD();
  SendChar(0x67);
  SendChar((x>>8)&0xff);
  SendChar(x&0xff);
  SendChar((y>>8)&0xff);
  SendChar(y&0xff);
  SendChar((r>>8)&0xff);
  SendChar(r&0xff);
  SendChar((sa>>8)&0xff);
  SendChar(sa&0xff);
  SendChar((ea>>8)&0xff);
  SendChar(ea&0xff);
  SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� GUI_Rectangle()
* ��    �ܣ� �����ľ���
* ��ڲ����� x0      �Ե�Ϊ��λ�Ŀ��ľ������Ͻ�X����ֵ
             y0      �Ե�Ϊ��λ�Ŀ��ľ������Ͻ�Y����ֵ   
             x1      �Ե�Ϊ��λ�Ŀ��ľ������½�X����ֵ
             y1      �Ե�Ϊ��λ�Ŀ��ľ������½�Y����ֵ   
* ���ڲ����� ��
****************************************************************************/
void GUI_Rectangle(uint16_t x0, uint16_t y0, uint16_t x1,uint16_t y1 )
{
  SendBeginCMD();
  SendChar(0x54);
  SendChar((x0>>8)&0xff);
  SendChar(x0&0xff);
  SendChar((y0>>8)&0xff);
  SendChar(y0&0xff);
  SendChar((x1>>8)&0xff);
  SendChar(x1&0xff);
  SendChar((y1>>8)&0xff);
  SendChar(y1&0xff);
  SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� RectangleFill()
* ��    �ܣ� ��ʵ�ľ���
* ��ڲ����� x0      �Ե�Ϊ��λ��ʵ�ľ������Ͻ�X����ֵ
             y0      �Ե�Ϊ��λ��ʵ�ľ������Ͻ�Y����ֵ   
             x1      �Ե�Ϊ��λ��ʵ�ľ������½�X����ֵ
             y1      �Ե�Ϊ��λ��ʵ�ľ������½�Y����ֵ   
* ���ڲ����� ��
****************************************************************************/
void GUI_RectangleFill(uint16_t x0, uint16_t y0, uint16_t x1,uint16_t y1 )
{
  SendBeginCMD();
  SendChar(0x55);
  SendChar((x0>>8)&0xff);
  SendChar(x0&0xff);
  SendChar((y0>>8)&0xff);
  SendChar(y0&0xff);
  SendChar((x1>>8)&0xff);
  SendChar(x1&0xff);
  SendChar((y1>>8)&0xff);
  SendChar(y1&0xff);
  SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� GUI_AreaInycolor()
* ��    �ܣ� ��Ļ����ɫ
* ��ڲ����� x0      �Ե�Ϊ��λ��ʵ�ľ������Ͻ�X����ֵ
             y0      �Ե�Ϊ��λ��ʵ�ľ������Ͻ�Y����ֵ   
             x1      �Ե�Ϊ��λ��ʵ�ľ������½�X����ֵ
             y1      �Ե�Ϊ��λ��ʵ�ľ������½�Y����ֵ   
* ���ڲ����� ��
****************************************************************************/
void GUI_AreaInycolor(uint16_t x0, uint16_t y0, uint16_t x1,uint16_t y1 )
{
  SendBeginCMD();
  SendChar(0x65);
  SendChar((x0>>8)&0xff);
  SendChar(x0&0xff);
  SendChar((y0>>8)&0xff);
  SendChar(y0&0xff);
  SendChar((x1>>8)&0xff);
  SendChar(x1&0xff);
  SendChar((y1>>8)&0xff);
  SendChar(y1&0xff);
  SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� GUI_Ellipse()
* ��    �ܣ� ��������Բ
* ��ڲ����� x0      �Ե�Ϊ��λ�Ŀ�����Բ�����X����ֵ
             y0      �Ե�Ϊ��λ�Ŀ�����Բ���϶�Y����ֵ   
             x1      �Ե�Ϊ��λ�Ŀ�����Բ���Ҷ�X����ֵ
             y1      �Ե�Ϊ��λ�Ŀ�����Բ���¶�Y����ֵ   
* ���ڲ����� ��
****************************************************************************/
void GUI_Ellipse (uint16_t x0, uint16_t y0, uint16_t x1,uint16_t y1 )
{
  SendBeginCMD();
  SendChar(0x56);
  SendChar((x0>>8)&0xff);
  SendChar(x0&0xff);
  SendChar((y0>>8)&0xff);
  SendChar(y0&0xff);
  SendChar((x1>>8)&0xff);
  SendChar(x1&0xff);
  SendChar((y1>>8)&0xff);
  SendChar(y1&0xff);
  SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� GUI_EllipseFill()
* ��    �ܣ� ��ʵ����Բ
* ��ڲ����� x0      �Ե�Ϊ��λ��ʵ����Բ�����X����ֵ
             y0      �Ե�Ϊ��λ��ʵ����Բ���϶�Y����ֵ   
             x1      �Ե�Ϊ��λ��ʵ����Բ���Ҷ�X����ֵ
             y1      �Ե�Ϊ��λ��ʵ����Բ���¶�Y����ֵ   
* ���ڲ����� ��
****************************************************************************/
void GUI_EllipseFill (uint16_t x0, uint16_t y0, uint16_t x1,uint16_t y1 )
{
  SendBeginCMD();
  SendChar(0x57);
  SendChar((x0>>8)&0xff);
  SendChar(x0&0xff);
  SendChar((y0>>8)&0xff);
  SendChar(y0&0xff);
  SendChar((x1>>8)&0xff);
  SendChar(x1&0xff);
  SendChar((y1>>8)&0xff);
  SendChar(y1&0xff);
  SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� GUI_PolygonFill()
* ��    �ܣ� ��������
* ��ڲ����� x               �Ե�Ϊ��λ��X������ֵ
             y               �Ե�Ϊ��λ��Y������ֵ    
             color            �����ɫ   
* ���ڲ����� ��
****************************************************************************/
void GUI_PolygonFill(uint16_t x, uint16_t y, uint16_t color)
{
  SendBeginCMD();
  SendChar(0x64);
  SendChar((x>>8)&0xff);
  SendChar(x&0xff);
  SendChar((y>>8)&0xff);
  SendChar(y&0xff);
  SendChar((color>>8)&0xff);
  SendChar(color&0xff);
  SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� SetBackLight()
* ��    �ܣ� �������
* ��ڲ����� light_level
* ���ڲ����� ��
****************************************************************************/
void SetBackLight(uint8_t light_level)
{
  SendBeginCMD();
  SendChar(0x60);
  SendChar(light_level);
  SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� SetBuzzer()
* ��    �ܣ� ����������
* ��ڲ����� light_level
* ���ڲ����� ��
****************************************************************************/
void SetBuzzer(uint8_t time)
{
  SendBeginCMD();
  SendChar(0x61);
  SendChar(time);
  SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� SetCommBps()
* ��    �ܣ� ����ͨѶ������
* ��ڲ����� Baud
* ���ڲ����� ��
****************************************************************************/
void SetCommBps(uint8_t baudset)  
{
  SendBeginCMD();
  SendChar(0xA0);
  SendChar(baudset);
  SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� SetTouchScreen(uint8_t cmd)
* ��    �ܣ� ����������
* ��ڲ����� cmd   Bit0:1/0 ������/��; Bit1 : 1/0 ��������/��; Bit2: �ϴ���ʽ  
* ���ڲ����� ��
****************************************************************************/
void SetTouchScreen(uint8_t cmd)
{
  SendBeginCMD();
  SendChar(0x70);
  SendChar(cmd);
  SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� SetTouchScreen_Adj()
* ��    �ܣ� ������У׼��У׼��Ϻ��·�������Ϣ
* ��ڲ����� ��
* ���ڲ����� ��
****************************************************************************/
void SetTouchScreen_Adj()
{
  SendBeginCMD();
  SendChar(0x72);
  SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� TestTouchScreen()
* ��    �ܣ� ����������
* ��ڲ����� ��
* ���ڲ����� ��
****************************************************************************/
void TestTouchScreen()
{
  SendBeginCMD();
  SendChar(0x73);
  SendEndCmd();
}

/****************************************************************************
* ��    �ƣ� WriteLayer()
* ��    �ܣ� дͼ�����
* ��ڲ����� layer         д���ͼ����
* ���ڲ����� ��
****************************************************************************/
void WriteLayer(uint8_t layer)
{
   SendBeginCMD();
   SendChar(0xa1);
   SendChar(layer);
   SendEndCmd();
}
/****************************************************************************
* ��    �ƣ� WriteLayer()
* ��    �ܣ� �л�ͼ����ʾ
* ��ڲ����� layer         �л���ʾ��ͼ��
* ���ڲ����� ��
****************************************************************************/
void DisplyLayer(uint8_t layer)
{
   SendBeginCMD();
   SendChar(0xa2);
   SendChar(layer);
   SendEndCmd();
}


 /****************************************************************************
* ��    �ƣ� SetScreen()
* ��    �ܣ� �л�����
* ��ڲ����� screen_id��Ŀ�껭��ID
* ���ڲ����� ��
****************************************************************************/
void SetScreen(uint16_t screen_id)
{
	SendBeginCMD();
	SendChar(0xb1);
	SendChar(0x00);
	SendChar(screen_id>>8);
	SendChar(screen_id&0xff);
	SendEndCmd();
}

/****************************************************************************
* ��    �ƣ� GetScreen()
* ��    �ܣ� �첽��ȡ��ǰ����ID
* ��ڲ����� ��
* ���ڲ����� ��
****************************************************************************/
void GetScreen()
{
	SendBeginCMD();
	SendChar(0xb1);
	SendChar(0x01);
	SendEndCmd();
}

/****************************************************************************
* ��    �ƣ� SetControlValue()
* ��    �ܣ� ���ÿؼ�����ֵ
* ��ڲ����� screen_id���ؼ����ڵĻ���ID
* ��ڲ����� control_id��Ŀ��ؼ�ID
* ��ڲ����� param�����ò���
* ��ڲ����� param_len����������
* ���ڲ����� ��
****************************************************************************/
void SetControlValue(uint16_t screen_id,uint16_t control_id,const char *param,uint16_t param_len)
{
	uint16_t i = 0;
	SendBeginCMD();
	SendChar(0xb1);
	SendChar(0x10);
	SendChar(screen_id>>8);
	SendChar(screen_id&0xff);
	SendChar((control_id>>8));
	SendChar((control_id&0xff));
	for (i=0;i<param_len;++i)
	{
	  SendChar(param[i]);
	}
	SendEndCmd();
}
void SetControlValue2(uint16_t screen_id,uint16_t control_id,const char *pre,const char *param,const char *post)
{
	SendBeginCMD();
	SendChar(0xb1);
	SendChar(0x10);
	SendChar(screen_id>>8);
	SendChar(screen_id&0xff);
	SendChar((control_id>>8));
	SendChar((control_id&0xff));
	if(pre){
		while(*pre){
			SendChar(*pre);
			pre++;
		}
	}
	if(param){
		while(*param){
			SendChar(*param);
			param++;
		}
	}
	if(post){
		while(*post){
			SendChar(*post);
			post++;
		}
	}
	SendEndCmd();
}

/****************************************************************************
* ��    �ƣ� SetButtonValue()
* ��    �ܣ� ���ð�ť�ؼ���״ֵ̬
* ��ڲ����� screen_id���ؼ����ڵĻ���ID
* ��ڲ����� control_id��Ŀ��ؼ�ID
* ��ڲ����� value����ť״̬-0����1����
* ���ڲ����� ��
****************************************************************************/
void SetButtonValue(uint16_t screen_id,uint16_t control_id,char value)
{
	SetControlValue(screen_id,control_id,&value,1);
}

/****************************************************************************
* ��    �ƣ� SetTextValue()
* ��    �ܣ� �����ı��ؼ�����ʾ����
* ��ڲ����� screen_id���ؼ����ڵĻ���ID
* ��ڲ����� control_id��Ŀ��ؼ�ID
* ��ڲ����� str����ʾ����
* ���ڲ����� ��
****************************************************************************/
void SetTextValue(uint16_t screen_id,uint16_t control_id,const char *str)
{
	uint16_t len = 0;
	const char *p = str;
	while(*p++) ++len;

	SetControlValue(screen_id,control_id,str,len);
}

static void SetProg_Meter_Slider_Value(uint16_t screen_id,uint16_t control_id,uint32_t value)
{
	SendBeginCMD();
	SendChar(0xb1);
	SendChar(0x10);
	SendChar(screen_id>>8);
	SendChar(screen_id&0xff);
	SendChar((control_id>>8));
	SendChar((control_id&0xff));
	SendChar((value>>24)&0xff);
	SendChar((value>>16)&0xff);
	SendChar((value>>8)&0xff);
	SendChar((value&0xff));
	SendEndCmd();
}

/****************************************************************************
* ��    �ƣ� SetProgressValue()
* ��    �ܣ� ���ý������ؼ�����ֵ
* ��ڲ����� screen_id���ؼ����ڵĻ���ID
* ��ڲ����� control_id��Ŀ��ؼ�ID
* ��ڲ����� value-�������Ľ���λ��
* ���ڲ����� ��
****************************************************************************/
void SetProgressValue(uint16_t screen_id,uint16_t control_id,uint32_t value)
{ 
    SetProg_Meter_Slider_Value(screen_id,control_id,value);
}

/****************************************************************************
* ��    �ƣ� SetMeterValue()
* ��    �ܣ� �����Ǳ�ؼ�����ֵ
* ��ڲ����� screen_id���ؼ����ڵĻ���ID
* ��ڲ����� control_id��Ŀ��ؼ�ID
* ��ڲ����� value-�Ǳ�ָ��λ��
* ���ڲ����� ��
****************************************************************************/
void SetMeterValue(uint16_t screen_id,uint16_t control_id,uint32_t value)
{
	SetProg_Meter_Slider_Value(screen_id,control_id,value);
}

/****************************************************************************
* ��    �ƣ� SetSliderValue()
* ��    �ܣ� ���û������ؼ�����ֵ
* ��ڲ����� screen_id���ؼ����ڵĻ���ID
* ��ڲ����� control_id��Ŀ��ؼ�ID
* ��ڲ����� value-�������Ļ���λ��
* ���ڲ����� ��
****************************************************************************/
void SetSliderValue(uint16_t screen_id,uint16_t control_id,uint32_t value)
{
	SetProg_Meter_Slider_Value(screen_id,control_id,value);
 }


/****************************************************************************
* ��    �ƣ� GetControlValue()
* ��    �ܣ� �첽��ȡ�ؼ���ֵ
* ��ڲ����� screen_id���ؼ����ڵĻ���ID
* ��ڲ����� control_id��Ŀ��ؼ�ID
* ���ڲ����� ��
****************************************************************************/
void GetControlValue(uint16_t screen_id,uint16_t control_id)
{
	SendBeginCMD();
	SendChar(0xb1);
	SendChar(0x11);
	SendChar(screen_id>>8);
	SendChar(screen_id&0xff);
	SendChar((control_id>>8));
	SendChar((control_id&0xff));
	SendEndCmd();
}
//EE B1 23 00 03 00 C8 63 FF FC FF FF 
void AnimationSetFrame(uint16_t sid,uint16_t cid,uint8_t frame){
	SendBeginCMD();
	SendChar(0xB1);
	SendChar(0x23);
	SendChar(sid>>8);
	SendChar(sid&0x00FF);
	SendChar(cid>>8);
	SendChar(cid&0x00FF);
	SendChar(frame);
	SendEndCmd();
}
void ShowControl(uint16_t sid,uint16_t cid,uint8_t enable){
	SendBeginCMD();
	SendChar(0xB1);
	SendChar(0x03);
	SendChar(sid>>8);
	SendChar(sid&0x00FF);
	SendChar(cid>>8);
	SendChar(cid&0x00FF);
	SendChar(enable);
	SendEndCmd();
}



