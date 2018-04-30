#include "oled_drv.h"
#include "nrf_drv_spi.h"
#include "nrf_gpio.h"
#include "app_util_platform.h"

#define NRF_LOG_MODULE_NAME "OLED"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"


#define SPI_INSTANCE 				2		// SPI实例定义

#define SPI2_CONFIG_SCK_PIN			14		// SPI2 SCK引脚定义
#define SPI2_CONFIG_MOSI_PIN		15		// SPI2 MOSI引脚定义
//#define SPI2_CONFIG_MISO_PIN
#define SPI2_CONFIG_IRQ_PRIORITY	APP_IRQ_PRIORITY_LOW	// SPI2中断优先级

/* LCD GPIO 相关配置 */
#define OLED_GPIO_CS		18
#define OLED_GPIO_DC		16
#define OLED_GPIO_REST		17
#define OLED_GPIO_LED_PWM	19

// 宏定义实现GPIO的高低控制
#define OLED_GPIO_CS_HIGH()		nrf_gpio_pin_set(OLED_GPIO_CS)
#define OLED_GPIO_CS_LOW()		nrf_gpio_pin_clear(OLED_GPIO_CS)

#define OLED_GPIO_DC_DAT()		nrf_gpio_pin_set(OLED_GPIO_DC)
#define OLED_GPIO_DC_CMD()		nrf_gpio_pin_clear(OLED_GPIO_DC)

// RESET low active
#define OLED_GPIO_REST_HIGH()		nrf_gpio_pin_set(OLED_GPIO_REST)
#define OLED_GPIO_REST_LOW()		nrf_gpio_pin_clear(OLED_GPIO_REST)

static volatile bool spi2_xfer_done;  /**< Flag used to indicate that SPI2 instance completed the transfer. */

static const nrf_drv_spi_t m_spi_master_2 = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);

static uint8_t rx_byte;
/**
 * @brief SPI2 user event handler.
 * @param event
 */
void spi2_event_handler(nrf_drv_spi_evt_t const * p_event)
{
    spi2_xfer_done = true;
    NRF_LOG_INFO("Transfer completed.\r\n");
}

static void spi_config(void)
{	
	NRF_LOG_INFO("SPI2 init.\r\n");
	
	nrf_drv_spi_config_t config = NRF_DRV_SPI_DEFAULT_CONFIG;
	config.sck_pin = SPI2_CONFIG_SCK_PIN;
	config.mosi_pin = SPI2_CONFIG_MOSI_PIN;
	config.frequency = NRF_DRV_SPI_FREQ_1M;
	config.mode = NRF_DRV_SPI_MODE_3;
	config.bit_order = NRF_DRV_SPI_BIT_ORDER_LSB_FIRST;
	
	APP_ERROR_CHECK(nrf_drv_spi_init(&m_spi_master_2, &config, spi2_event_handler));
}

static void oled_gpio_init(void)
{
	nrf_gpio_cfg_output(OLED_GPIO_CS);
	nrf_gpio_cfg_output(OLED_GPIO_REST);
	nrf_gpio_cfg_output(OLED_GPIO_DC);
	nrf_gpio_cfg_output(OLED_GPIO_LED_PWM);
}

static void oled_gpio_pwm_init(void)
{
	// 暂时不处理PWM控制，待确定屏幕可以点亮后处理
	nrf_gpio_pin_set(OLED_GPIO_LED_PWM);	// 直接将PWM拉高
}

static void spi2_oled_send_byte(uint8_t byte)
{
	NRF_LOG_INFO("SPI2 transfer.\r\n");
	
	APP_ERROR_CHECK(nrf_drv_spi_transfer(&m_spi_master_2, &byte, 1, &rx_byte, 1));
	
	while (!spi2_xfer_done)
    {
		__WFE();
    }
	
	NRF_LOG_FLUSH();
}

static void LCD_WriteCommand(uint8_t c)
{
	OLED_GPIO_CS_LOW();
	OLED_GPIO_DC_CMD();
	spi2_oled_send_byte(c);
	OLED_GPIO_CS_HIGH();
}

static void LCD_WriteData(uint8_t dat)
{
	OLED_GPIO_CS_LOW();
	OLED_GPIO_DC_DAT();
	spi2_oled_send_byte(dat);
	OLED_GPIO_CS_HIGH();
}

/* 写像素数据 18bit */
static void LCD_Write_Data(uint16_t dat16)
{
	LCD_WriteData(dat16 >> 8);
	LCD_WriteData(dat16);
}

/*
 *	OLED 初始化函数，对外接口
 *	在系统外设初始化阶段调用
 */
void oled_init(void)
{
	oled_gpio_init();
	oled_gpio_pwm_init();
	spi_config();
	
// ST7735S RESET Sequence
	OLED_GPIO_REST_HIGH();
	//delay 100
	OLED_GPIO_REST_LOW();
	// delay 100
	OLED_GPIO_REST_HIGH();
	// delay 300
	
	LCD_WriteCommand(0x11);		// sleep out & booster on
	// delay 120

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
	
	// delay 120
	LCD_WriteCommand(0x29);		// Display on
	
}

static void oled_set_window(uint8_t start_x, uint8_t end_x, 
							uint8_t start_y, uint8_t end_y)
{
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
 *	3、16 * 16
 */

/*
 * OLED写点函数
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
	
	while(temp_x--)		// 对X画点
	{
		temp = temp_y;
		
		while(temp--)		// 对Y画点
		{
			LCD_Write_Data(color);
		}
	}
}

/* 
 *	显示GBK编码中文
 *	参数 p : 要显示的中文字模，从字库中获得，应为32位字节
*	参数 back_color: 字体显示的背景色，若不知，填入NULL
 */
void oled_disp_word(uint8_t x,
					uint8_t y, 
					uint8_t *p,
					uint16_t word_color,
					uint16_t back_color)
{
	uint8_t i = 0, j = 0, z = 0;
	uint8_t color;
	
	oled_set_window(x, x + 15, y, y + 15);		// 字高：16 * 16
	
	for(z = 2; z > 0; z--)
	{
		for(j = 16; j > 0; j--)
		{
			color = *p;
			for(i = 8; i > 0; i--)		// 循环判断字模中byte各位是否为1
			{
				if(((color) & 0x80) == 0x80) {		// 判断字模bit位是否为1，为1则显示
					LCD_Write_Data(word_color);
					} else {							// 字模bit位为0，显示背景色
						if(back_color != NULL) {		// 若背景色不知，填入参数NULL
							LCD_Write_Data(back_color);
						}					
				}
				
				color <<= 1;
			}
		}
	}
}
