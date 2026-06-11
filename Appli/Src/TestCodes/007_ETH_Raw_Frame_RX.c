/*
 * 007_ETH_Raw_Frame_RX.c
 *
 *  Created on: 11 Haz 2026
 *      Author: talha
 */

#include <stdint.h>
#include <stdio.h>
#include "stm32h7sxx.h"
#include "stm32h7sxx_gpio_driver.h"
#include "stm32h7sxx_eth_driver.h"

volatile uint8_t  dbg_frame_available = 0U;
volatile uint32_t dbg_length = 0U;
volatile uint32_t dbg_desc_index = 0U;
volatile uint32_t dbg_desc3 = 0U;
volatile uint32_t dbg_frame_addr = 0U;

volatile uint32_t dbg_dmacrxcr = 0U;
volatile uint32_t dbg_dmacrxdlar = 0U;
volatile uint32_t dbg_dmacrxrlr = 0U;
volatile uint32_t dbg_dmacrxdtpr = 0U;
volatile uint32_t dbg_maccr = 0U;
volatile uint32_t dbg_macpfr = 0U;
volatile uint32_t dbg_mtlrxqomr = 0U;

void Test_007_ETH_Raw_Frame_RX(void){

	uint8_t phy_addr = 0U;
	uint8_t phy_found = 0U;
	uint8_t eth_reset_ok = 0U;

	uint8_t mac[6] = {
			0x02U, 0x00U, 0x00U, 0x00U, 0x00U, 0x01U
	};

	uint8_t *frame = 0U;
	uint32_t length = 0U;
	uint8_t frame_available = 0U;

	/*
	 * Non-cacheable ETH DMA Memory
	 */
	ETH_MPU_ConfigNonCacheable();

	/*
	 * Basic ETH init
	 */
	ETH_PeriClockControl(ENABLE);
	ETH_GPIO_RMII_Init();
	ETH_SelectRMII();

	eth_reset_ok = ETH_SoftwareReset();

	if(eth_reset_ok == 0U){
		while(1);
	}

	/*
	 * Phy Check
	 */
	phy_found = ETH_PHY_FindAddress(&phy_addr);

	if(phy_found == 0U) {
		while(1);
	}

	if(ETH_PHY_WaitForLink(phy_addr, ETH_TIMEOUT_VALUE) == 0U){
		while(1);
	}

	if(ETH_PHY_WaitAutoNegotiation(phy_addr, ETH_TIMEOUT_VALUE) == 0U){
		while(1);
	}

	/*
	 * MAC Address;
	 */
	ETH_SetMACAddress(mac);

	/*
	 * Promiscuous mode for testing
	 */
	ETH_MAC_FilterConfigForTest();

	/*
	 * Init RX Descriptors
	 */
	ETH_RXDesc_Init();

	/*
	 * Configure MTL and DMA RX
	 */
	ETH_MTL_RXQueueConfigRSF();
	ETH_DMA_RXConfig();

	/*
	 * Start RX
	 */
	ETH_DMA_RXStart();
	ETH_MAC_RXEnable();

	while(1){
		length = 0U;
		frame = 0;

		frame_available = ETH_ReadRawFrame(&frame, &length);

		dbg_frame_available = frame_available;
		dbg_length = length;
		dbg_desc_index = ETH_GetRXCurrentDescIndex();
		dbg_desc3 = ETH_GetRXDesc3(dbg_desc_index);
		dbg_frame_addr = (uint32_t)frame;

		dbg_dmacrxcr = ETH->DMACRXCR;
		dbg_dmacrxdlar = ETH->DMACRXDLAR;
		dbg_dmacrxrlr = ETH->DMACRXRLR;
		dbg_dmacrxdtpr = ETH->DMACRXDTPR;
		dbg_maccr = ETH->MACCR;
		dbg_macpfr = ETH->MACPFR;
		dbg_mtlrxqomr = ETH->MTLRXQOMR;

		if(frame_available == 1U)
		{
			/*
			 * Breakpoint here.
			 */
			ETH_ReleaseRXDescriptor();
		}
	}
}
