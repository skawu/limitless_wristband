#ifndef _W25Q128_DRV_H_
#define _W25Q128_DRV_H_

#include "nrf_drv_spi.h"
#include "nrf_gpio.h"

// W25Qxx系列芯片ID
#define W25Q80	0xEF13
#define W25Q16	0xEF14
#define W25Q32	0xEF15
#define W25Q64	0xEF16
#define W25Q128	0xEF17


// w25qxx操作指令表
#define W25Qxx_WriteEnable			0x06
#define W25Qxx_WriteDisable			0x04
#define W25Qxx_ReadStatusReg		0x05
#define W25Qxx_WriteStatusReg		0x01
#define W25Qxx_ReadData				0x03
#define W25Qxx_FastReadData			0x0B
#define W25Qxx_FastReadDual			0x3B
#define W25Qxx_PageProgram			0x02
#define W25Qxx_BlockErase			0xD8
#define W25Qxx_SectorErase			0x20
#define W25Qxx_ChipErase			0xC7
#define W25Qxx_PowerDown			0xB9
#define W25Qxx_ReleasePowerDown		0xAB
#define W25Qxx_DeviceId				0xAB
#define W25Qxx_ManufactDeviceID		0x90
#define W25Qxx_JedecDeviceID		0x9F

extern void spi_config(void);
extern uint16_t W25Qxx_ReadID(void);

#endif
