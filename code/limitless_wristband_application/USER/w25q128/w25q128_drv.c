#include "w25q128_drv.h"

#include "app_util_platform.h"
#include "FreeRTOS.h"

/*
 *	</ w25q128 硬件相关定义
 */
 #define FLASH_GPIO_CS		25
 #define FLASH_GPIO_CLK		24
 #define FLASH_GPIO_MOSI	27
 #define FLASH_GPIO_MISO	26
 
 // 宏定义实现GPIO的高低控制
#define FLASH_GPIO_CS_HIGH()			nrf_gpio_pin_set(FLASH_GPIO_CS)
#define FLASH_GPIO_CS_LOW()				nrf_gpio_pin_clear(FLASH_GPIO_CS)

 /* </ end w25q128 硬件相关定义 */

/*
 *	</ w25q128 SPI1 相关参数定义
 */
 #define FLASH_SPI_INSTANCE 				1		// SPI实例定义
 static const nrf_drv_spi_t m_spi_master_1 = NRF_DRV_SPI_INSTANCE(FLASH_SPI_INSTANCE);
 
 static volatile bool spi1_xfer_done;  /**< Flag used to indicate that SPI1 instance completed the transfer. */
 
static uint8_t       m_tx_buf[6];           /**< TX buffer. */
static uint8_t       m_rx_buf[260];    		/**< RX buffer. */
//static const uint8_t m_tx_length = sizeof(m_tx_buf);        /**< Transfer length. */
//static const uint8_t m_rx_length = sizeof(m_rx_buf);        /**< Transfer length. */ 

  /* </ end w25q128 相关参数定义 */

/**
 * @brief SPI1 user event handler.
 * @param event
 */
void spi1_event_handler(nrf_drv_spi_evt_t const * p_event)
{
	spi1_xfer_done = true;
}

void spi_config(void)
{
	nrf_drv_spi_config_t config = NRF_DRV_SPI_DEFAULT_CONFIG;
//	config.ss_pin = FLASH_GPIO_CS;
	config.sck_pin = FLASH_GPIO_CLK;
	config.mosi_pin = FLASH_GPIO_MOSI;
	config.miso_pin = FLASH_GPIO_MISO;
	config.frequency = NRF_DRV_SPI_FREQ_4M;
	config.mode = NRF_DRV_SPI_MODE_0;
	config.bit_order = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST;
	
	APP_ERROR_CHECK(nrf_drv_spi_init(&m_spi_master_1, &config, spi1_event_handler));
}

/* </ Flash SPI 外设关闭函数，用于低功耗优化 */
void FLASH_spi_close(void)
{
	nrf_drv_spi_uninit(&m_spi_master_1);
}

/* </ FLASH SPI 读写 */
static void spi1_flash_Wite_Read(uint8_t *tx_dat, uint8_t tx_len, uint8_t *rx_dat, uint8_t rx_len)
{
	spi1_xfer_done = false;

	APP_ERROR_CHECK(nrf_drv_spi_transfer(&m_spi_master_1, tx_dat, tx_len, rx_dat, rx_len + tx_len));

	while (!spi1_xfer_done) {		// 后续while改为信号量，不使用while阻塞
		__WFE();
	}
}


/********************************** W25Qxx操作 *******************************************/

// 读W25Qxx状态寄存器
uint8_t W25Qxx_ReadSR(void)
{
	uint8_t byte_write = W25Qxx_ReadStatusReg, byte_read = 0;
	
	FLASH_GPIO_CS_LOW();
	spi1_flash_Wite_Read(&byte_write, 1, &byte_read, 1);
	FLASH_GPIO_CS_HIGH();
	
	return byte_read;
}

// 写W25Qxx状态寄存器
// 注意：只有SRP0，TB，BP2，BP1，BP0 （bit7,5,4,3,2）可以写！！！
void W25Qxx_WriteSR(uint8_t dat)
{
	uint8_t byte_write = W25Qxx_WriteStatusReg, byte_read = 0;
	
	FLASH_GPIO_CS_LOW();	
	spi1_flash_Wite_Read(&byte_write, 1, &byte_read, 0);
	spi1_flash_Wite_Read(&dat, 1, &byte_read, 0);
	FLASH_GPIO_CS_HIGH();
	
}

// W25Qxx写使能，将WEL置位
void W25Qxx_Write_Enable(void)
{
	uint8_t byte_write = W25Qxx_WriteEnable, byte_read = 0;
	
	FLASH_GPIO_CS_LOW();
	spi1_flash_Wite_Read(&byte_write, 1, &byte_read, 0);
	FLASH_GPIO_CS_HIGH();
}

// W25Qxx写禁止，将WEL清零
void W25Qxx_Write_Disable(void)
{
	uint8_t byte_write = W25Qxx_WriteDisable, byte_read = 0;
	
	FLASH_GPIO_CS_LOW();
	spi1_flash_Wite_Read(&byte_write, 1, &byte_read, 0);
	FLASH_GPIO_CS_HIGH();
}

// 等待空闲
void W25Qxx_Wait_Busy(void)
{
	while((W25Qxx_ReadSR() & 0x01) == 0x01);
}

// 进入掉电模式
void W25Qxx_Power_Down(void)
{
	uint16_t i = 0x02FF;
	m_tx_buf[0] = W25Qxx_PowerDown;
	
	FLASH_GPIO_CS_LOW();
	spi1_flash_Wite_Read(m_tx_buf, 1, NULL, 0);
	FLASH_GPIO_CS_HIGH();
	
	while(i--);
}

// 唤醒
void W25Qxx_Wake_Up()
{
	uint16_t i = 0x02FF;
	m_tx_buf[0] = W25Qxx_ReleasePowerDown;
	
	FLASH_GPIO_CS_LOW();
	spi1_flash_Wite_Read(m_tx_buf, 1, NULL, 0);
	FLASH_GPIO_CS_HIGH();
	
	while(i--);
}

// 读取芯片ID
uint16_t W25Qxx_ReadID(void)
{
	uint16_t temp = 0;
	m_tx_buf[0] = 0x90;
	m_tx_buf[1] = 0x00;
	m_tx_buf[2] = 0x00;
	m_tx_buf[3] = 0x00;
	
	FLASH_GPIO_CS_LOW();	
	spi1_flash_Wite_Read(m_tx_buf, 4, m_rx_buf, 2);
	FLASH_GPIO_CS_HIGH();
		
	temp = m_rx_buf[4] << 8;
	temp |= m_rx_buf[5];
	
	return temp;
}

// 读取SPI FLASH，在指定地址开始读取指定长度的数据
// pBuffer：读取数据存放区
// ReadAddr： 要读取数据的起始地址(24bit)
// DataLen： 要读取的数据长度
void W25Qxx_Read(uint8_t *pBuffer, uint32_t ReadAddr, uint8_t DataLen)
{
	uint8_t i = 0;
	m_tx_buf[0] = W25Qxx_ReadData;
	m_tx_buf[1] = (uint8_t)(ReadAddr >> 16);
	m_tx_buf[2] = (uint8_t)(ReadAddr >> 8);
	m_tx_buf[3] = (uint8_t)(ReadAddr);
	
	FLASH_GPIO_CS_LOW();
	spi1_flash_Wite_Read(m_tx_buf, 4, m_rx_buf, DataLen);
	FLASH_GPIO_CS_HIGH();
	
	// 数据转储
	for(i = 0; i < DataLen; i++)
	{
		pBuffer[i] = m_rx_buf[4 + i];
	}
}

// 擦除整个芯片
void W25Qxx_Erase_Chip(void)
{
	W25Qxx_Write_Enable();	
	W25Qxx_Wait_Busy();
	
	m_tx_buf[0] = W25Qxx_ChipErase;
	
	FLASH_GPIO_CS_LOW();
	spi1_flash_Wite_Read(m_tx_buf, 1, NULL, 0);
	FLASH_GPIO_CS_HIGH();
	
	W25Qxx_Wait_Busy();
}

// 擦除一个扇区
void W25Qxx_Erase_Sector(uint32_t Addr)
{
	Addr *= 4096;
	W25Qxx_Write_Enable();	
	W25Qxx_Wait_Busy();
	
	m_tx_buf[0] = W25Qxx_SectorErase;
	m_tx_buf[1] = (uint8_t)(Addr >> 16);
	m_tx_buf[2] = (uint8_t)(Addr >> 8);
	m_tx_buf[3] = (uint8_t)Addr;
	
	FLASH_GPIO_CS_LOW();
	spi1_flash_Wite_Read(m_tx_buf, 4, NULL, 0);
	FLASH_GPIO_CS_HIGH();
	
	W25Qxx_Wait_Busy();		// 等待擦除完成
	
}

// 在一页（block 0xFFFF）中写少于256字节的数据，不能跨页去写
//
void W25Qxx_Wirte_Page(uint8_t *pBuffer, uint32_t WriteAddr, uint8_t DataLen)
{
	m_tx_buf[0] = W25Qxx_PageProgram;
	m_tx_buf[1] = (uint8_t)(WriteAddr >> 16);
	m_tx_buf[2] = (uint8_t)(WriteAddr >> 8);
	m_tx_buf[3] = (uint8_t)(WriteAddr);
	
	W25Qxx_Write_Enable();
	
	FLASH_GPIO_CS_LOW();
	spi1_flash_Wite_Read(m_tx_buf, 4, NULL, 0);
	spi1_flash_Wite_Read(pBuffer, DataLen, NULL, 0);
	FLASH_GPIO_CS_HIGH();
	
	W25Qxx_Wait_Busy();
}
