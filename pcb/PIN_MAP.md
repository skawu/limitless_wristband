##pin map

|引脚		|功能		|片上外设		|功能模块		|
|:----------|:----------|:--------------|:--------------|
|P0.03		|ADC_IN2	|ADC			|battery_level	|
|			|			|				|				|
|P0.04		|VIBRATION	|GPIOTE			|vibration motor|
|			|			|				|				|
|P0.10		|SPI_CLK	|SPI0			|MPU6050		|
|P0.11		|SPI_CS		|SPI0			|MPU6050		|
|P0.12		|SPI_MISO	|SPI0			|MPU6050		|
|P0.13		|SPI_MOSI	|SPI0			|MPU6050		|
|P0.09		|INT		|GPIOTE			|MPU6050		|
|			|			|				|				|
|P0.24		|SPI_CLK	|SPI1			|SPI FLASH		|
|P0.25		|SPI_CS		|SPI1			|SPI FLASH		|
|P0.26		|SPI_MISO	|SPI1			|SPI FLASH		|
|P0.27		|SPI_MOSI	|SPI1			|SPI FLASH		|
|			|			|				|				|
|P0.14		|IIC_CLK	|IIC1			|TFT_LCD		|
|P0.15		|IIC_SDA	|IIC1			|TFT_LCD		|
|P0.16		|D/C sel	|GPIO			|TFT_LCD		|
|P0.17		|RESET		|GPIO			|TFT_LCD		|
|P0.18		|CS			|GPIO			|TFT_LCD		|
|P0.19		|LEDA		|GPIO			|TFT_LCD		|
|			|			|				|				|
|P0.20		|IIC_CLK	|IIC0			|HRS3300		|
|P0.21		|IIC_SDA	|IIC0			|HRS3300		|
|P0.22		|INT		|GPIOTE			|HRS3300		|
|P0.23		|VLEDA		|GPIO			|HRS3300		|
|			|			|				|				|

### 注意
1. TFT_LCD 的 LEDA脚电流有15ma，不可直接使用NRF52832的IO口驱动。
