/*
 *	file: gui_drv.h
 *	describe: 主要用来实现GUI的显示操作包括
 *				1、ASCII字符显示
 *				2、GBK汉字显示
 *				3、图片显示
 *				4、GIF动画显示
 *				5、GUI的初始化
 */

/*
 *	字符显示格式定义：
 *	1、取模方式：行列式。即一列一列的扫描显示字符，扫描（16）列
 *	2、点阵格式：阴码。
 *	3、汉字为 16 * 16
 *	4、取模走向：顺向
 */
 
#include <stdio.h>
#include "gui_drv.h"
#include "oled_drv.h"
#include "w25q128_drv.h"
#include "asciicode.h"



/* 清屏 */
void GUI_clear(void)
{
	oled_wirte_box(0, WIDTH, 0, HEIGHT, BLACK);		// 初始化清屏为黑色
}

/* GUI 初始化 */
void GUI_init(void)
{
	oled_init();
	GUI_clear();
	GUI_showImage(0, 0, (uint8_t *)gImage_10);	
	GUI_Show12ASCII(20, 10, (uint8_t *)" Bpm ", BLUE, BLACK);
	GUI_ShowChinese16X16(40, 40, (uint8_t *)hz10492, BLUE, BLACK);
}

/****************************************************************************
* Function Name  : GUI_Show12ASCII
* Description    : 写12号ASCII码
* Input          : x：起始X坐标
*                * y：起始Y坐标
*                * p：字符串首地址
*                * wordColor：字体颜色
*                * backColor：背景颜色
* Output         : None
* Return         : None
****************************************************************************/

void GUI_Show12ASCII(uint16_t x, uint16_t y, uint8_t *p,
                     uint16_t wordColor, uint16_t backColor)
{
	uint8_t i, wordByte, wordNum;
	uint16_t color;

	while (*p != '\0')  //检测是否是最后一个字
	{
		/* 在字库中的ASCII码是从空格开始的也就是32开始的，所以减去32 */
		wordNum = *p - 32;
		oled_set_window(x, x + 7, y, y + 15);       //字宽*高为：8*16

		for (wordByte = 0; wordByte < 16; wordByte++) //每个字模一共有16个字节，先画上半部分8个字节
		{
			color = ASCII8x16[wordNum][wordByte];

			for (i = 0; i < 8; i++)
			{
				if ((color & 0x80) == 0x80)
				{
					LCD_Write_Data(wordColor);
				}
				else
				{
					LCD_Write_Data(backColor);
				}

				color <<= 1;
			}
		}
		

		p++;    //指针指向下一个字
		/* 屏幕坐标处理 */
		x += 8;

		if (x > (WIDTH - 7))  //TFT_XMAX -8
		{
			x = 0;
			y += 16;
		}
	}
}

/****************************************************************************
* Function Name  : GUI_Show12Char
* Description    : 通过FLASH字库显示8*16字母和16*16汉字（使用GBK）
* Input          : x：起始X坐标
*                * y：起始Y坐标
*                * ch：字符串首地址
*                * wordColor：字体颜色
*                * backColor：背景颜色
* Output         : None
* Return         : None
****************************************************************************/

void GUI_Show16Char(uint16_t x, uint16_t y, uint8_t *ch,
                    uint16_t wordColor, uint16_t backColor)
{
	uint8_t i, j, color, wordNum, wordByte, buf[32];
	uint16_t asc;
	uint32_t wordAddr = 0;

	while (*ch != '\0')
	{
		/*显示字母 */
		if (*ch < 0x80) //ASCII码从0~127
		{
					/* 在字库中的ASCII码是从空格开始的也就是32开始的，所以减去32 */
		wordNum = *ch - 32;
		oled_set_window(x, x + 7, y, y + 15);       //字宽*高为：8*16

		for (wordByte = 0; wordByte < 16; wordByte++) //每个字模一共有16个字节，先画上半部分8个字节
		{
			color = ASCII8x16[wordNum][wordByte];

			for (i = 0; i < 8; i++)
			{
				if ((color & 0x80) == 0x80)
				{
					LCD_Write_Data(wordColor);
				}
				else
				{
					LCD_Write_Data(backColor);
				}

				color <<= 1;
			}
		}
		

		ch++;    //指针指向下一个字
		/* 屏幕坐标处理 */
		x += 8;

		if (x > (WIDTH - 7))  //TFT_XMAX -8
		{
			x = 0;
			y += 16;
		}
		}
		/* 显示汉字 */
		else
		{
			/* 将汉字GBK编码转换成在FLASH中的地址 */
			asc = *ch - 0x81;     	// 高字节是表示分区，分区是从0x81到0xFE,所以转换成地址-0x80
			wordAddr = asc * 192; 	// 每个分区一共有190个字
			asc = *(ch + 1); 		// 低字节代表每个字在每个分区的位置，它是从0x40到0xFF
			asc -= 0x40;			// 0x7F位置补位，字库中空余的位置补位，所以在字库中可以线性查找

			wordAddr += asc; //求出在GBK中是第几个字
			wordAddr *= 32;  //将字位置转换位FLASH地址
			wordAddr += GUI_FLASH_16CHAR_ADDR; //加上首地址
			
			/* 读取FLASH中该字的字模 */
			W25Qxx_Read(buf, wordAddr, 32);
			
			/* 在彩屏上面显示 */
			oled_set_window(x, x + 15, y, y + 15);

			for (i = 0; i < 32; i++)
			{
				color = buf[i];

				for (j = 0; j < 8; j++)
				{
					if ((color & 0x80) == 0x80)
					{
						LCD_Write_Data(wordColor);
					}
					else
					{
						LCD_Write_Data(backColor);
					}

					color <<= 1;
				}//for(j=0;j<8;j++)结束
			}

			/* 屏幕坐标处理 */
			x += 16;

			if (x > (WIDTH - 15))  //TFT_XMAX -15
			{
				x = 0;
				y += 16;
			}

			/* 写下一个字，每个汉字占两个字节所以+2 */
			ch += 2;
		}
	}
}

/****************************************************************************
* Function Name  : GUI_ShowChinese16X16
* Description    : 显示一个GBK编码格式16x16的汉字
* Input          : x：起始X坐标
*                * y：起始Y坐标
*                * p：字符串首地址
*                * wordColor：字体颜色
*                * backColor：背景颜色
* Output         : None
* Return         : None
****************************************************************************/

void GUI_ShowChinese16X16(uint16_t x, uint16_t y, uint8_t *p,
                     uint16_t wordColor, uint16_t backColor)
{
	uint8_t i, wordByte;	
	uint16_t color;

	oled_set_window(x, x + 15, y, y + 15);       	// 字宽*高为：16 * 16

	for (wordByte = 0; wordByte < 32; wordByte++) 	// 每个字模一共有32个字节
	{
		color = p[wordByte];

		for (i = 0; i < 8; i++)
		{
			if ((color & 0x80) == 0x80)
			{
				LCD_Write_Data(wordColor);
			}
			else
			{
				LCD_Write_Data(backColor);
			}

			color <<= 1;
		}
	}
}

/****************************************************************************
* Function Name  : GUI_showImage
* Description    : 显示一张图片
* Input          : StartX：起始X坐标
*                * StartY：起始Y坐标
*                * pic：字符串首地址
* Output         : None
* Return         : None
****************************************************************************/
void GUI_showImage(uint8_t StartX,uint8_t StartY, uint8_t *pic)
{
	uint32_t  i=8, len;
	uint8_t temp,x,y; 
	
	x=((uint16_t)(pic[2]<<8) + pic[3]) - 1;     							// 从图像数组里取出图像的长度  
	y=((uint16_t)(pic[4]<<8) + pic[5]) - 1;     							// 从图像数组里取出图像的高度 
		
	oled_set_window(StartX, StartX + x, StartY, StartY + y);
	
	len=2*((uint16_t)(pic[2]<<8)+pic[3])*((uint16_t)(pic[4]<<8)+pic[5]);   // 计算出图像所占的字节数  
	
	while(i<(len+8)) 													   // 从图像数组的第9位开始递增  
	{                          						   
		temp=(uint16_t)( pic[i]<<8)+pic[i+1];      						   // 16位总线， 需要一次发送2个字节的数据  
		LCD_Write_Data(temp);               							   // 将取出的16位像素数据送入显示区  
		i=i+2;                       									   // 取模位置加2，以为获取下一个像素数据  
	}
		
}
