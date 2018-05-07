#include "w25q128_drv.h"

#include "nrf_drv_spi.h"
#include "nrf_gpio.h"
#include "app_util_platform.h"
#include "FreeRTOS.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"

/*
 *	</ w25q128 硬件相关定义
 */
 #define FLASH_GPIO_CS		25
 #define FLASH_GPIO_CLK		24
 #define FLASH_GPIO_MOSI	27
 #define FLASH_GPIO_MISO	26
 
 /* </ end w25q128 硬件相关定义 */

/*
 *	</ w25q128 SPI1 相关参数定义
 */
 #define FLASH_SPI_INSTANCE 				1		// SPI实例定义
 static const nrf_drv_spi_t m_spi_master_1 = NRF_DRV_SPI_INSTANCE(FLASH_SPI_INSTANCE);
 
 static volatile bool spi1_xfer_done;  /**< Flag used to indicate that SPI1 instance completed the transfer. */
 
static uint8_t       m_tx_buf[5];           /**< TX buffer. */
static uint8_t       m_rx_buf[40];    		/**< RX buffer. */
static const uint8_t m_tx_length = sizeof(m_tx_buf);        /**< Transfer length. */
static const uint8_t m_rx_length = sizeof(m_rx_buf);        /**< Transfer length. */ 

  /* </ end w25q128 相关参数定义 */

/**
 * @brief SPI1 user event handler.
 * @param event
 */
void spi1_event_handler(nrf_drv_spi_evt_t const * p_event)
{
	spi1_xfer_done = true;
	NRF_LOG_INFO("Transfer completed.\r\n");
}

static void spi_config(void)
{
	NRF_LOG_INFO("SPI2 init.\r\n");

	nrf_drv_spi_config_t config = NRF_DRV_SPI_DEFAULT_CONFIG;
	config.ss_pin = NRF_DRV_SPI_PIN_NOT_USED;
	config.sck_pin = FLASH_GPIO_CLK;
	config.mosi_pin = FLASH_GPIO_MOSI;
	config.miso_pin = FLASH_GPIO_MISO;
	config.frequency = NRF_DRV_SPI_FREQ_1M;
	config.mode = NRF_DRV_SPI_MODE_3;
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
	NRF_LOG_INFO("SPI1 transfer.\r\n");
	spi1_xfer_done = false;

	APP_ERROR_CHECK(nrf_drv_spi_transfer(&m_spi_master_1, tx_dat, tx_len, rx_dat, rx_len));

	while (!spi1_xfer_done) {		// 后续while改为信号量，不使用while阻塞
		__WFE();
	}

	NRF_LOG_FLUSH();
}
