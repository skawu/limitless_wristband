#ifndef _OLED_DRV_H_
#define _OLED_DRV_H_

#include "stdint.h"

#define HEIGHT	159
#define WIDTH	79

#define RED     0xf800
#define GREEN   0x07e0
#define BLUE   0x001f
#define YELLOW 0xffe0
#define WHITE   0xffff
#define BLACK   0x0000
#define PURPLE 0xf81f

extern void oled_init(void);
extern void oled_write_dot(uint8_t x, uint8_t y, uint16_t color);
extern void oled_wirte_box(uint8_t start_x, uint8_t end_x, uint8_t start_y, uint8_t end_y, uint16_t color);
extern void oled_disp_word(uint8_t x, uint8_t y, uint8_t *p, uint16_t word_color, uint16_t back_color);

#endif
