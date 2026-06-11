/*
 * 006_ETH_DMA_Memory_Init.c
 *
 *  Created on: 8 Haz 2026
 *      Author: talha
 */
#include <stdint.h>
#include "stm32h7sxx.h"
#include "stm32h7sxx_gpio_driver.h"
#include "stm32h7sxx_eth_driver.h"

void Test_006_ETH_DMA_Memory_Init(void){
	ETH_DMADesc_t *rx_desc_base;
	uint8_t *rx_buff_base;

	uint8_t mac[6] = {
			0x02U, 0x00U, 0x00U, 0x00U, 0x00U, 0x01U  // 02:00:00:00:00:01
	};

	/*
	 * Configure 0x24040000 - 0x24043FFF as non-cacheable memory.
	 *
	 * This should be called before Ethernet DMA uses RX descriptors/buffers.
	 */
	ETH_MPU_ConfigNonCacheable();

	rx_desc_base = ETH_GetRXDescBase();
	rx_buff_base = ETH_GetRXBufferBase();

	ETH1_PCLK_EN();
	/*
	 * Set STM32 MAC Address
	 */
	ETH_SetMACAddress(mac);

	/*
	 * RX descriptors init
	 */
	ETH_RXDesc_Init();

	while(1){
		/*
		 * Debug checks:
		 *
		 * rx_desc_base = 0x24040000
		 * rx_buf_base  = 0x24040040
		 *
		 * ETH_RX_Desc[0].DESC0 = 0x24040040
		 * ETH_RX_Desc[0].DESC3 = 0xC1000000
		 *
		 * ETH->MACA0LR and ETH->MACA0HR should contain MAC address.
		 */
	}
}


