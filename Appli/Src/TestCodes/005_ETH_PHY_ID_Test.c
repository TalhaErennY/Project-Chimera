/*
 * 005_ETH_PHY_ID_Test.c
 *
 *  Created on: 7 Haz 2026
 *      Author: talha
 */

#include <stdint.h>
#include "stm32h7sxx.h"
#include "stm32h7sxx_gpio_driver.h"
#include "stm32h7sxx_eth_driver.h"

void Test_005_ETH_PHY_ID(void){
	uint8_t phy_addr  = 0U;
	uint8_t phy_found = 0U;
	uint8_t link_up   = 0U;
	uint8_t auto_neg_complete = 0U;
	uint8_t link_wait_ok = 0U;
	uint8_t auto_neg_wait_ok = 0U;
	uint8_t eth_reset_ok = 0U;

	uint16_t phy_id1  = 0U;
	uint16_t phy_id2  = 0U;
	uint16_t phy_bsr  = 0U;

	/*
	 * ETH Peripheral clocks
	 *
	 * ETH MAC, ETH TX, ETH RX Clock bits
	 */
	ETH_PeriClockControl(ENABLE);

	/*
	 * RMII GPIO Pin Configuration
	 */
	ETH_GPIO_RMII_Init();

	/*
	 * Select Ethernet PHY interface as RMII
	 */
	ETH_SelectRMII();

	/*
	 * Ethernet software reset (if stuck check ETH Clock and RMII_REF_CLK
	 */
	ETH_SoftwareReset();

	/*
	 * Find PHY Address on MDIO bus
	 */
	phy_found = ETH_PHY_FindAddress(&phy_addr);

	if(phy_found == 1U){
		/*
		 * Read PHY ID registers
		 */
		phy_id1 = ETH_PHY_Read(phy_addr, PHY_REG_ID1);
		phy_id2 = ETH_PHY_Read(phy_addr, PHY_REG_ID2);

		/*
		 * Read PHY status register
		 */
		phy_bsr = ETH_PHY_Read(phy_addr, PHY_REG_BSR);

		/*
		 * Check Ethernet link status
		 */
		link_up = ETH_PHY_IsLinkUp(phy_addr);

		/*
		 * Wait for ethernet link
		 */
		link_wait_ok = ETH_PHY_WaitForLink(phy_addr, ETH_TIMEOUT_VALUE);

		/*
		 * Wait for auto-negatiation
		 */
		auto_neg_wait_ok = ETH_PHY_WaitAutoNegotiation(phy_addr, ETH_TIMEOUT_VALUE);

		/*
		 * Read final status
		 */
		phy_bsr = ETH_PHY_Read(phy_addr, PHY_REG_BSR);
		link_up = ETH_PHY_IsLinkUp(phy_addr);
		auto_neg_complete = ETH_PHY_IsAutoNegComplete(phy_addr);

	}

	ETH_DMADesc_t* rx_desc_base;
	uint8_t* rx_buf_base;

	rx_desc_base = ETH_GetRXDescBase();
	rx_buf_base = ETH_GetRXBufferBase();

	while(1){
		/*
		 * check variables for working status
		 *
		 * phy_found = 1
		 * phy_id1 != 0x0000
		 * phy_id1 != 0xFFFF
		 * phy_id2 != 0x0000
		 * phy_id2 != 0xFFFF
		 */
	}

}

