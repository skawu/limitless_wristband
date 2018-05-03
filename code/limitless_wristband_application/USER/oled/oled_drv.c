#include "oled_drv.h"
#include "nrf_drv_spi.h"
#include "nrf_gpio.h"
#include "app_util_platform.h"
#include "FreeRTOS.h"

#define NRF_LOG_MODULE_NAME "OLED"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"


#define OLED_SPI_INSTANCE 				2		// SPI实例定义

#define SPI2_CONFIG_SCK_PIN			22 // 14		// SPI2 SCK引脚定义
#define SPI2_CONFIG_MOSI_PIN		23 // 15		// SPI2 MOSI引脚定义

/* LCD GPIO 相关配置 */
#define OLED_GPIO_CS		25 // 18
#define OLED_GPIO_DC		26 // 16
#define OLED_GPIO_REST		24 // 17
#define OLED_GPIO_LED_PWM	27 //19

// 宏定义实现GPIO的高低控制
#define OLED_GPIO_CS_HIGH()			nrf_gpio_pin_set(OLED_GPIO_CS)
#define OLED_GPIO_CS_LOW()			nrf_gpio_pin_clear(OLED_GPIO_CS)

#define OLED_GPIO_DC_DAT()			nrf_gpio_pin_set(OLED_GPIO_DC)
#define OLED_GPIO_DC_CMD()			nrf_gpio_pin_clear(OLED_GPIO_DC)

#define OLED_GPIO_SCK_HIGH()		nrf_gpio_pin_set(SPI2_CONFIG_SCK_PIN)
#define OLED_GPIO_SCK_LOW()			nrf_gpio_pin_clear(SPI2_CONFIG_SCK_PIN)

#define OLED_GPIO_SDA_HIGH()		nrf_gpio_pin_set(SPI2_CONFIG_MOSI_PIN)
#define OLED_GPIO_SDA_LOW()			nrf_gpio_pin_clear(SPI2_CONFIG_MOSI_PIN)

// RESET low active
#define OLED_GPIO_REST_HIGH()		nrf_gpio_pin_set(OLED_GPIO_REST)
#define OLED_GPIO_REST_LOW()		nrf_gpio_pin_clear(OLED_GPIO_REST)



static void oled_gpio_init(void)
{
	nrf_gpio_cfg_output(OLED_GPIO_CS);
	nrf_gpio_cfg_output(OLED_GPIO_REST);
	nrf_gpio_cfg_output(OLED_GPIO_DC);
	nrf_gpio_cfg_output(OLED_GPIO_LED_PWM);
	nrf_gpio_cfg_output(SPI2_CONFIG_SCK_PIN);
	nrf_gpio_cfg_output(SPI2_CONFIG_MOSI_PIN);
}

static void oled_gpio_pwm_init(void)
{
	// 暂时不处理PWM控制，待确定屏幕可以点亮后处理
	nrf_gpio_pin_set(OLED_GPIO_LED_PWM);	// 直接将PWM拉高
}


static void LCD_WriteCommand(uint8_t c)
{
	char i;
	OLED_GPIO_CS_LOW();
	OLED_GPIO_DC_CMD();
	for(i=0;i<8;i++)
	{
		OLED_GPIO_SCK_LOW();
		if(c & 0x80) OLED_GPIO_SDA_HIGH();
		else OLED_GPIO_SDA_LOW();
		OLED_GPIO_SCK_HIGH();
		c = c<<=1;
	}
}

static void LCD_WriteData(uint8_t dat)
{
	char i;
	OLED_GPIO_CS_LOW();
	OLED_GPIO_DC_DAT();
	for(i=0;i<8;i++)
	{
		OLED_GPIO_SCK_LOW();
		if(dat & 0x80) OLED_GPIO_SDA_HIGH();
		else OLED_GPIO_SDA_LOW();
		OLED_GPIO_SCK_HIGH();
		dat = dat<<=1;
	}
}

void Delay(int count)   /* X10ms */
{
	unsigned int i,j;
	for(i=0; i<count; i++)
		for(j=0; j<50; j++);
}

/* 写像素数据 16bit */
static void LCD_Write_Data(uint16_t dat16)
{
	LCD_WriteData((uint8_t)((dat16 & 0xff00) >> 8));
	LCD_WriteData((uint8_t)(dat16 & 0x00ff));
}

/*
 *	OLED 初始化函数，对外接口
 *	在系统外设初始化阶段调用
 */
void oled_init(void)
{
	oled_gpio_init();
	oled_gpio_pwm_init();

// ST7735S RESET Sequence
	OLED_GPIO_REST_HIGH();
	Delay(100); 
	
	OLED_GPIO_REST_LOW();
	Delay(100); 
	
	OLED_GPIO_REST_HIGH();
	Delay(300); 

	LCD_WriteCommand(0x11);		// sleep out & booster on
	Delay(300); 

// Frame Rate
	LCD_WriteCommand(0xB1);		// Frame Rate Control (In normal mode / Full colors)
	LCD_WriteData(0x05);
	LCD_WriteData(0x3C);
	LCD_WriteData(0x3C);

	LCD_WriteCommand(0xB2);
	LCD_WriteData(0x05);
	LCD_WriteData(0x3C);
	LCD_WriteData(0x3C);

	LCD_WriteCommand(0xB3);
	LCD_WriteData(0x05);
	LCD_WriteData(0x3C);
	LCD_WriteData(0x3C);
	LCD_WriteData(0x05);
	LCD_WriteData(0x3C);
	LCD_WriteData(0x3C);

	LCD_WriteCommand(0xB4);
	LCD_WriteData(0x03);

	LCD_WriteCommand(0xC0);
	LCD_WriteData(0x0E);
	LCD_WriteData(0x0E);
	LCD_WriteData(0x04);

	LCD_WriteCommand(0xC1);
	LCD_WriteData(0xC5);

	LCD_WriteCommand(0xC2);
	LCD_WriteData(0x0D);
	LCD_WriteData(0x00);

	LCD_WriteCommand(0xC3);
	LCD_WriteData(0x8D);
	LCD_WriteData(0x2A);

	LCD_WriteCommand(0xC4);
	LCD_WriteData(0x8D);
	LCD_WriteData(0xEE);

	LCD_WriteCommand(0xC5);
	LCD_WriteData(0x06);

	LCD_WriteCommand(0x36);		// Memory data access control
	LCD_WriteData(0xC8);

	LCD_WriteCommand(0x3A);		// Interface pixel format
	LCD_WriteData(0x55);

	LCD_WriteCommand(0xE0);
	LCD_WriteData(0x0B);
	LCD_WriteData(0x17);
	LCD_WriteData(0x0A);
	LCD_WriteData(0x0D);
	LCD_WriteData(0x1A);
	LCD_WriteData(0x19);
	LCD_WriteData(0x16);
	LCD_WriteData(0x1D);
	LCD_WriteData(0x21);
	LCD_WriteData(0x26);
	LCD_WriteData(0x37);
	LCD_WriteData(0x3C);
	LCD_WriteData(0x00);
	LCD_WriteData(0x09);
	LCD_WriteData(0x05);
	LCD_WriteData(0x10);

	LCD_WriteCommand(0xE1);
	LCD_WriteData(0x0C);
	LCD_WriteData(0x19);
	LCD_WriteData(0x09);
	LCD_WriteData(0x0D);
	LCD_WriteData(0x1B);
	LCD_WriteData(0x19);
	LCD_WriteData(0x15);
	LCD_WriteData(0x1D);
	LCD_WriteData(0x21);
	LCD_WriteData(0x26);
	LCD_WriteData(0x39);
	LCD_WriteData(0x3E);
	LCD_WriteData(0x00);
	LCD_WriteData(0x09);
	LCD_WriteData(0x05);
	LCD_WriteData(0x10);

	LCD_WriteCommand(0x3A); 	//65k mode
	LCD_WriteData(0x05);
	
	LCD_WriteCommand(0x29);		// Display on


	oled_wirte_box(0, WIDTH, 0, HEIGHT, BLACK);		// 初始化清屏为黑色
}

static void oled_set_window(uint8_t start_x, uint8_t end_x,
                            uint8_t start_y, uint8_t end_y)
{
	start_x = start_x + 24;		// X轴方向有 24 pixel的补偿值
	end_x = end_x + 24;
	
	LCD_WriteCommand(0x2A);		// 设置列地址
	LCD_WriteData(0x00);
	LCD_WriteData(start_x);		// 列起始地址
	LCD_WriteData(0x00);
	LCD_WriteData(end_x);		// 列结束地址

	LCD_WriteCommand(0x2B);		// 设置行地址
	LCD_WriteData(0x00);
	LCD_WriteData(start_y);		// 行起始地址
	LCD_WriteData(0x00);
	LCD_WriteData(end_y);		// 行结束地址

	LCD_WriteCommand(0x2C);
}


/*
 *	字符显示格式定义：
 *	1、列行式。即一列一列的扫描显示字符，扫描（16）列
 *	2、阴码。
*	3、汉字为 16 * 16
 */

/*
 * OLED画点函数
 */
void oled_write_dot(uint8_t x,
                    uint8_t y,
                    uint16_t color)
{
	oled_set_window(x, x, y, y);

	LCD_Write_Data(color);	// 画点
}

/*
 *	OLED 给一个区域涂上颜色
 */
void oled_wirte_box(uint8_t start_x, uint8_t end_x,
                    uint8_t start_y, uint8_t end_y,
                    uint16_t color)
{
	uint8_t temp_x, temp_y, temp;

	if((start_x > end_x) || (start_y > end_y)) {	// 判断所画区域定义是否正确
		return ;
	}

	oled_set_window(start_x, end_x, start_y, end_y);

	temp_x = end_x - start_x + 1;
	temp_y = end_y - start_y + 1;

	while(temp_y--) {	// 对X画点
		temp = temp_x;

		while(temp--) {	// 对Y画点
			LCD_Write_Data(color);
		}
	}
}
