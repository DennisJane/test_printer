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
* 名    称： SendBeginCMD()
* 功    能： 发送帧头命令
* 入口参数： 无      
* 出口参数： 无
****************************************************************************/
void SendBeginCMD()
{
 SendChar(0xEE);
}

/****************************************************************************
* 名    称： SendEndCmd()
* 功    能： 发送帧尾命令
* 入口参数： 无      
* 出口参数： 无
****************************************************************************/
void SendEndCmd()
{
 SendChar(0xFF);
 SendChar(0xFC);
 SendChar(0xFF);
 SendChar(0xFF);
}

/****************************************************************************
* 名    称： SetHandShake()
* 功    能： 握手
* 入口参数： 无      
* 出口参数： 无
****************************************************************************/
void SetHandShake(void)
{
  SendBeginCMD();
  SendChar(0x00);
  SendEndCmd();
}


/****************************************************************************
* 名    称： SetFcolor()
* 功    能： 设置前景色
* 入口参数： fcolor  前景色RGB参数   
* 出口参数： 无
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
* 名    称： SetBcolor()
* 功    能： 设置背景色
* 入口参数： bcolor  背景色RGB参数   
* 出口参数： 无
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
* 名    称： ColorPicker()
* 功    能： 取屏幕点设置前景色/背景色
* 入口参数： mode 取当前显示屏幕(X,Y)处的颜色作为前景/背景色
             x       以点为单位的X轴坐标值
             y       以点为单位的Y轴坐标值
* 出口参数： 无
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
* 名    称： GUI_CleanScreen()
* 功    能： 清屏
* 入口参数： 无  
* 出口参数： 无
****************************************************************************/
void GUI_CleanScreen()
{
  SendBeginCMD();
  SendChar(0x01);
  SendEndCmd();
}

/****************************************************************************
* 名    称： SetTextSpace()
* 功    能： 设置文字行列间距
* 入口参数： x_w     以点为单位的行间距，取值范围00~1F
             y_w     以点为单位的列间距，取值范围00~1F	
* 出口参数： 无
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
* 名    称： DisText_Region()
* 功    能： 限定文本显示范围
* 入口参数： enable   限制文本使能
             length   文本框限定的长度
	     width    文本框限定的高度
* 出口参数： 无
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
* 名    称： SetFilterColor()
* 功    能： 设置过滤颜色
* 入口参数： fillcolor_dwon   滤除颜色的下限值
             fillcolor_up     滤除颜色的上限值	
* 出口参数： 无
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
* 名    称： DisText()
* 功    能： 文本显示
* 入口参数： x         以点为单位的X轴坐标值
             y         以点为单位的Y轴坐标值
   	         back      背景色使能
	         font      字库编码
             strings   写入的字符串
* 出口参数： 无
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
* 名    称： DisCursor()
* 功    能： 光标显示
* 入口参数： enable
             x        以点为单位的X轴坐标值
             y        以点为单位的Y轴坐标值
             length   光标长度
	         width    光标宽度
* 出口参数： 无
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
* 名    称： DisFull_Image()
* 功    能： 全屏整幅图显示
* 入口参数： image_id   图片ID编号
             masken     屏蔽色使能
* 出口参数： 无
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
* 名    称： DisArea_Image()
* 功    能： 区域图片显示
* 入口参数： x          以点为单位的X轴坐标值
             y          以点为单位的Y轴坐标值 
             image_id   图片ID编号
             masken     屏蔽色使能
* 出口参数： 无
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
* 名    称： DisCut_Image()
* 功    能： 图片剪切
* 入口参数： x          以点为单位的X轴坐标值
             y          以点为单位的Y轴坐标值 
             image_id   图片编号
             image_x    图片内部X坐标
             image_y    图片内部Y坐标
             image_l    剪切长度
             image_w    剪切宽度
             masken     屏蔽色使能
* 出口参数： 无
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
* 名    称： DisFlashImgae()
* 功    能： 动画显示
* 入口参数： x               以点为单位的X轴坐标值
             y               以点为单位的Y轴坐标值 
             flashimage_id   GIF动画ID编号     
    	     enable          关闭/开启动作播放
	         playnum         动画播放次数 0:一直播放，1~FF:播放次数
* 出口参数： 无
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
* 名    称： GUI_Dot()
* 功    能： 画点
* 入口参数： x       以点为单位的X轴坐标值
             y       以点为单位的Y轴坐标值           
* 出口参数： 无
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
* 名    称： GUI_Line()
* 功    能： 画线
* 入口参数： x0      以点为单位的X轴坐标值
             y0      以点为单位的Y轴坐标值    
             x1      以点为单位的X轴坐标值
             y1      以点为单位的Y轴坐标值  
* 出口参数： 无
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
* 名    称： GUI_ConDots()
* 功    能： 绘制折线,将指定的坐标点自动连接起来
* 入口参数： mode     0:用前景色描绘 1：用背景色描绘    
	         pDot     指向点的指针   
			 num      数据的个数  
* 出口参数： 无
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
* 名    称： GUI_Circle()
* 功    能： 画空心圆
* 入口参数： x               以点为单位的X轴坐标值
             y               以点为单位的Y轴坐标值    
             r                空心圆的半径   
* 出口参数： 无
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
* 名    称： GUI_CircleFill()
* 功    能： 画实心圆
* 入口参数： x               以点为单位的X轴坐标值
             y               以点为单位的Y轴坐标值    
             r                实心圆的半径   
* 出口参数： 无
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
* 名    称： GUI_Arc()
* 功    能： 画圆弧
* 入口参数： x               以点为单位的X轴坐标值
             y               以点为单位的Y轴坐标值    
             r               圆的半径  
 	         sa              起始角度
	         ea              结束角度
* 出口参数： 无
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
* 名    称： GUI_Rectangle()
* 功    能： 画空心矩形
* 入口参数： x0      以点为单位的空心矩形左上角X坐标值
             y0      以点为单位的空心矩形左上角Y坐标值   
             x1      以点为单位的空心矩形右下角X坐标值
             y1      以点为单位的空心矩形右下角Y坐标值   
* 出口参数： 无
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
* 名    称： RectangleFill()
* 功    能： 画实心矩形
* 入口参数： x0      以点为单位的实心矩形左上角X坐标值
             y0      以点为单位的实心矩形左上角Y坐标值   
             x1      以点为单位的实心矩形右下角X坐标值
             y1      以点为单位的实心矩形右下角Y坐标值   
* 出口参数： 无
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
* 名    称： GUI_AreaInycolor()
* 功    能： 屏幕区域反色
* 入口参数： x0      以点为单位的实心矩形左上角X坐标值
             y0      以点为单位的实心矩形左上角Y坐标值   
             x1      以点为单位的实心矩形右下角X坐标值
             y1      以点为单位的实心矩形右下角Y坐标值   
* 出口参数： 无
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
* 名    称： GUI_Ellipse()
* 功    能： 画空心椭圆
* 入口参数： x0      以点为单位的空心椭圆最左端X坐标值
             y0      以点为单位的空心椭圆最上端Y坐标值   
             x1      以点为单位的空心椭圆最右端X坐标值
             y1      以点为单位的空心椭圆最下端Y坐标值   
* 出口参数： 无
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
* 名    称： GUI_EllipseFill()
* 功    能： 画实心椭圆
* 入口参数： x0      以点为单位的实心椭圆最左端X坐标值
             y0      以点为单位的实心椭圆最上端Y坐标值   
             x1      以点为单位的实心椭圆最右端X坐标值
             y1      以点为单位的实心椭圆最下端Y坐标值   
* 出口参数： 无
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
* 名    称： GUI_PolygonFill()
* 功    能： 多边形填充
* 入口参数： x               以点为单位的X轴坐标值
             y               以点为单位的Y轴坐标值    
             color            填充颜色   
* 出口参数： 无
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
* 名    称： SetBackLight()
* 功    能： 背光调节
* 入口参数： light_level
* 出口参数： 无
****************************************************************************/
void SetBackLight(uint8_t light_level)
{
  SendBeginCMD();
  SendChar(0x60);
  SendChar(light_level);
  SendEndCmd();
}
/****************************************************************************
* 名    称： SetBuzzer()
* 功    能： 蜂鸣器控制
* 入口参数： light_level
* 出口参数： 无
****************************************************************************/
void SetBuzzer(uint8_t time)
{
  SendBeginCMD();
  SendChar(0x61);
  SendChar(time);
  SendEndCmd();
}
/****************************************************************************
* 名    称： SetCommBps()
* 功    能： 设置通讯波特率
* 入口参数： Baud
* 出口参数： 无
****************************************************************************/
void SetCommBps(uint8_t baudset)  
{
  SendBeginCMD();
  SendChar(0xA0);
  SendChar(baudset);
  SendEndCmd();
}
/****************************************************************************
* 名    称： SetTouchScreen(uint8_t cmd)
* 功    能： 触摸屏控制
* 入口参数： cmd   Bit0:1/0 触摸开/闭; Bit1 : 1/0 蜂鸣器开/闭; Bit2: 上传方式  
* 出口参数： 无
****************************************************************************/
void SetTouchScreen(uint8_t cmd)
{
  SendBeginCMD();
  SendChar(0x70);
  SendChar(cmd);
  SendEndCmd();
}
/****************************************************************************
* 名    称： SetTouchScreen_Adj()
* 功    能： 触摸屏校准，校准完毕后下发结束信息
* 入口参数： 无
* 出口参数： 无
****************************************************************************/
void SetTouchScreen_Adj()
{
  SendBeginCMD();
  SendChar(0x72);
  SendEndCmd();
}
/****************************************************************************
* 名    称： TestTouchScreen()
* 功    能： 触摸屏体验
* 入口参数： 无
* 出口参数： 无
****************************************************************************/
void TestTouchScreen()
{
  SendBeginCMD();
  SendChar(0x73);
  SendEndCmd();
}

/****************************************************************************
* 名    称： WriteLayer()
* 功    能： 写图层操作
* 入口参数： layer         写入的图层数
* 出口参数： 无
****************************************************************************/
void WriteLayer(uint8_t layer)
{
   SendBeginCMD();
   SendChar(0xa1);
   SendChar(layer);
   SendEndCmd();
}
/****************************************************************************
* 名    称： WriteLayer()
* 功    能： 切换图层显示
* 入口参数： layer         切换显示的图层
* 出口参数： 无
****************************************************************************/
void DisplyLayer(uint8_t layer)
{
   SendBeginCMD();
   SendChar(0xa2);
   SendChar(layer);
   SendEndCmd();
}


 /****************************************************************************
* 名    称： SetScreen()
* 功    能： 切换画面
* 入口参数： screen_id，目标画面ID
* 出口参数： 无
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
* 名    称： GetScreen()
* 功    能： 异步获取当前画面ID
* 入口参数： 无
* 出口参数： 无
****************************************************************************/
void GetScreen()
{
	SendBeginCMD();
	SendChar(0xb1);
	SendChar(0x01);
	SendEndCmd();
}

/****************************************************************************
* 名    称： SetControlValue()
* 功    能： 设置控件的数值
* 入口参数： screen_id，控件所在的画面ID
* 入口参数： control_id，目标控件ID
* 入口参数： param，设置参数
* 入口参数： param_len，参数长度
* 出口参数： 无
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
* 名    称： SetButtonValue()
* 功    能： 设置按钮控件的状态值
* 入口参数： screen_id，控件所在的画面ID
* 入口参数： control_id，目标控件ID
* 入口参数： value，按钮状态-0弹起，1按下
* 出口参数： 无
****************************************************************************/
void SetButtonValue(uint16_t screen_id,uint16_t control_id,char value)
{
	SetControlValue(screen_id,control_id,&value,1);
}

/****************************************************************************
* 名    称： SetTextValue()
* 功    能： 设置文本控件的显示文字
* 入口参数： screen_id，控件所在的画面ID
* 入口参数： control_id，目标控件ID
* 入口参数： str，显示文字
* 出口参数： 无
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
* 名    称： SetProgressValue()
* 功    能： 设置进度条控件的数值
* 入口参数： screen_id，控件所在的画面ID
* 入口参数： control_id，目标控件ID
* 入口参数： value-进度条的进度位置
* 出口参数： 无
****************************************************************************/
void SetProgressValue(uint16_t screen_id,uint16_t control_id,uint32_t value)
{ 
    SetProg_Meter_Slider_Value(screen_id,control_id,value);
}

/****************************************************************************
* 名    称： SetMeterValue()
* 功    能： 设置仪表控件的数值
* 入口参数： screen_id，控件所在的画面ID
* 入口参数： control_id，目标控件ID
* 入口参数： value-仪表指针位置
* 出口参数： 无
****************************************************************************/
void SetMeterValue(uint16_t screen_id,uint16_t control_id,uint32_t value)
{
	SetProg_Meter_Slider_Value(screen_id,control_id,value);
}

/****************************************************************************
* 名    称： SetSliderValue()
* 功    能： 设置滑动条控件的数值
* 入口参数： screen_id，控件所在的画面ID
* 入口参数： control_id，目标控件ID
* 入口参数： value-滑动条的滑块位置
* 出口参数： 无
****************************************************************************/
void SetSliderValue(uint16_t screen_id,uint16_t control_id,uint32_t value)
{
	SetProg_Meter_Slider_Value(screen_id,control_id,value);
 }


/****************************************************************************
* 名    称： GetControlValue()
* 功    能： 异步获取控件数值
* 入口参数： screen_id，控件所在的画面ID
* 入口参数： control_id，目标控件ID
* 出口参数： 无
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



