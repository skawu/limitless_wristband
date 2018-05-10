/*
 *	file: gui_drv.h
 *	describe: ��Ҫ����ʵ��GUI����ʾ��������
 *				1��ASCII�ַ���ʾ
 *				2��GBK������ʾ
 *				3��ͼƬ��ʾ
 *				4��GIF������ʾ
 *				5��GUI�ĳ�ʼ��
 */

/*
 *	�ַ���ʾ��ʽ���壺
 *	1��ȡģ��ʽ������ʽ����һ��һ�е�ɨ����ʾ�ַ���ɨ�裨16����
 *	2�������ʽ�����롣
 *	3������Ϊ 16 * 16
 *	4��ȡģ����˳��
 */
 
 
#include "gui_drv.h"
#include "oled_drv.h"
#include "asciicode.h"


/* ���� */
void GUI_clear(void)
{
	oled_wirte_box(0, WIDTH, 0, HEIGHT, BLACK);		// ��ʼ������Ϊ��ɫ
}

/* GUI ��ʼ�� */
void GUI_init(void)
{
	oled_init();
	GUI_clear();
	GUI_showImage(20, 20, (uint8_t *)gImage_bt);
//	GUI_Show12ASCII(0, 0, " zhe shi sha yan  se ?", WHITE, BLACK);
}

/****************************************************************************
* Function Name  : GUI_Show12ASCII
* Description    : д12��ASCII��
* Input          : x����ʼX����
*                * y����ʼY����
*                * p���ַ����׵�ַ
*                * wordColor��������ɫ
*                * backColor��������ɫ
* Output         : None
* Return         : None
****************************************************************************/

void GUI_Show12ASCII(uint16_t x, uint16_t y, uint8_t *p,
                     uint16_t wordColor, uint16_t backColor)
{
	uint8_t i, wordByte, wordNum;
	uint16_t color;

	while (*p != '\0')  //����Ƿ������һ����
	{
		/* ���ֿ��е�ASCII���Ǵӿո�ʼ��Ҳ����32��ʼ�ģ����Լ�ȥ32 */
		wordNum = *p - 32;
		oled_set_window(x, x + 7, y, y + 15);       //�ֿ�*��Ϊ��8*16

		for (wordByte = 0; wordByte < 16; wordByte++) //ÿ����ģһ����16���ֽڣ��Ȼ��ϰ벿��8���ֽ�
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
		

		p++;    //ָ��ָ����һ����
		/* ��Ļ���괦�� */
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
* Description    : ͨ��FLASH�ֿ���ʾ12����ĸ�ͺ��֣�ʹ��GBK��
* Input          : x����ʼX����
*                * y����ʼY����
*                * ch���ַ����׵�ַ
*                * wordColor��������ɫ
*                * backColor��������ɫ
* Output         : None
* Return         : None
****************************************************************************/

void GUI_Show12Char(uint16_t x, uint16_t y, uint8_t *ch,
                    uint16_t wordColor, uint16_t backColor)
{
	uint8_t i, j, color, buf[32];
	uint16_t asc;
	uint32_t wordAddr = 0;

	while (*ch != '\0')
	{
		/*��ʾ��ĸ */
		if (*ch < 0x80) //ASCII���0~127
		{
			/* ���ֿ��е�ASCII���Ǵӿո�ʼ��Ҳ����32��ʼ�ģ����Լ�ȥ32 */
			wordAddr = *ch - 32;
			wordAddr *= 16;
			wordAddr += GUI_FLASH_ASCII_ADDR;
			
			/* ��ȡFLASH�и��ֵ���ģ */
//			FLASH_ReadData(buf, wordAddr, 16);
			
			/* ��ʾ������ */
			oled_set_window(x, x + 7, y, y + 15);       //�ֿ�*��Ϊ��8*16

			for (j = 0; j < 16; j++) //ÿ����ģһ����16���ֽ�
			{
				color = buf[j];

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

			ch++;    //ָ��ָ����һ����
			/* ��Ļ���괦�� */
			x += 8;

			if (x > (WIDTH - 7))  //TFT_XMAX -8
			{
				x = 0;
				y += 16;
			}
		}
		/* ��ʾ���� */
		else
		{
			/* �����ֱ���ת������FLASH�еĵ�ַ */
			asc = *ch - 0x81;     //���ֽ��Ǳ�ʾ�����������Ǵ�0x81��0xFE,����ת���ɵ�ַ-0x80
			wordAddr = asc * 190; //ÿ������һ����190����
			asc = *(ch + 1); //���ֽڴ���ÿ������ÿ��������λ�ã����Ǵ�0x40��0xFF

			if (asc < 0x7F)  //��0x7Fλ���и���λ����������ȡģ�����գ����Դ���0x7F֮����һ
			{
				asc -= 0x40;
			}
			else
			{
				asc -= 0x41;
			}

			wordAddr += asc; //�����GBK���ǵڼ�����
			wordAddr *= 32;  //����λ��ת��λFLASH��ַ
			wordAddr += GUI_FLASH_12CHAR_ADDR; //�����׵�ַ
			
			/* ��ȡFLASH�и��ֵ���ģ */
//			FLASH_ReadData(buf, wordAddr, 32);
			
			/* �ڲ���������ʾ */
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
				}//for(j=0;j<8;j++)����
			}

			/* ��Ļ���괦�� */
			x += 16;

			if (x > (WIDTH - 15))  //TFT_XMAX -15
			{
				x = 0;
				y += 16;
			}

			/* д��һ���֣�ÿ������ռ�����ֽ�����+2 */
			ch += 2;
		}
	}
}


void GUI_showImage(uint8_t StartX,uint8_t StartY, uint8_t *pic)
{
	uint32_t  i=8, len;
	uint8_t temp,x,y; 
	
	x=((uint16_t)(pic[2]<<8)+pic[3])-1;     //��ͼ��������ȡ��ͼ��ĳ���  
	y=((uint16_t)(pic[4]<<8)+pic[5])-1;     //��ͼ��������ȡ��ͼ��ĸ߶� 
	
	LCD_WriteCommand(0x2A);
	//�����е�ַ
	LCD_WriteData(0x00);
	LCD_WriteData(StartX);
	LCD_WriteData(0x00);
	LCD_WriteData(StartX + x);

	LCD_WriteCommand(0x2B);
	//�����е�ַ
	LCD_WriteData(0x00);
	LCD_WriteData(StartY);
	LCD_WriteData(0x00);
	LCD_WriteData(StartY + y);
	
	LCD_WriteCommand(0x2C);
	
	len=2*((uint16_t)(pic[2]<<8)+pic[3])*((uint16_t)(pic[4]<<8)+pic[5]);   //�����ͼ����ռ���ֽ���  
	
	while(i<(len+8)) {                          //��ͼ������ĵ�9λ��ʼ����  
		temp=(uint16_t)( pic[i]<<8)+pic[i+1];      //16λ���ߣ� ��Ҫһ�η���2���ֽڵ�����  
		LCD_Write_Data(temp);               //��ȡ����16λ��������������ʾ��  
		i=i+2;                       //ȡģλ�ü�2����Ϊ��ȡ��һ����������  
  }
	
	
}
