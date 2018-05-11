#ifndef _GUI_DRV_H_
#define _GUI_DRV_H_

#include <stdint.h>

/* 设置字库地址 */
#define GUI_FLASH_ASCII_ADDR     6112846  //ASCII字库首地址（6114304 - 1456 - 2）
#define GUI_FLASH_12CHAR_ADDR    6114304  //12号字库首地址（6880386 - 766080 - 2）
#define GUI_FLASH_16CHAR_ADDR    6880386  //16号字库首地址（8388608 - 1508220 - 2）


void GUI_init(void);
extern void GUI_Show12ASCII(uint16_t x, uint16_t y, uint8_t *p, uint16_t wordColor, uint16_t backColor);
extern void GUI_showImage(uint8_t StartX, uint8_t StartY, uint8_t *pic);

extern const unsigned char gImage_000[];
extern const unsigned char gImage_001[];
extern const unsigned char gImage_002[];

#endif
