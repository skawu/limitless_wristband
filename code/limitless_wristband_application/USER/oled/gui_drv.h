#ifndef _GUI_DRV_H_
#define _GUI_DRV_H_

#include <stdint.h>

#define RED     0xf800
#define GREEN   0x07e0
#define BLUE   	0x001f
#define YELLOW 	0xffe0
#define WHITE   0xffff
#define BLACK   0x0000
#define PURPLE 	0xf81f

/* 设置字库地址 */
#define GUI_FLASH_ASCII_ADDR     6112846  //ASCII字库首地址（6114304 - 1456 - 2）
#define GUI_FLASH_12CHAR_ADDR    0  //12号字库首地址（6880386 - 766080 - 2）
#define GUI_FLASH_16CHAR_ADDR    0  //16号字库首地址（8388608 - 1508220 - 2）


void GUI_init(void);
extern void GUI_Show12ASCII(uint16_t x, uint16_t y, uint8_t *p, uint16_t wordColor, uint16_t backColor);
extern void GUI_Show16Char(uint16_t x, uint16_t y, uint8_t *ch, uint16_t wordColor, uint16_t backColor);
extern void GUI_ShowChinese16X16(uint16_t x, uint16_t y, uint8_t *p, uint16_t wordColor, uint16_t backColor);
extern void GUI_showImage(uint8_t StartX, uint8_t StartY, uint8_t *pic);

extern const unsigned char gImage_000[];
extern const unsigned char gImage_001[];
extern const unsigned char gImage_002[];

#endif
