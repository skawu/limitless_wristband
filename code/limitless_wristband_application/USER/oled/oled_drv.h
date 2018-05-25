#ifndef _OLED_DRV_H_
#define _OLED_DRV_H_

#include "nrf_drv_spi.h"
#include "nrf_gpio.h"

#define HEIGHT	159
#define WIDTH	79



extern void oled_init(void);
extern void LCD_WriteData(uint8_t dat);
extern void LCD_WriteCommand(uint8_t c);
extern void LCD_Write_Data(uint16_t dat16);
extern void oled_set_window(uint8_t start_x, uint8_t end_x, uint8_t start_y, uint8_t end_y);
extern void oled_write_dot(uint8_t x, uint8_t y, uint16_t color);
extern void oled_wirte_box(uint8_t start_x, uint8_t end_x, uint8_t start_y, uint8_t end_y, uint16_t color);
extern void oled_disp_word(uint8_t x, uint8_t y, uint8_t *p, uint16_t word_color, uint16_t back_color);

#endif
