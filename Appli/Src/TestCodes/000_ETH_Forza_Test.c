/*
 * 000_ETH_Forza_Test.c
 *
 *  Created on: 11 Haz 2026
 *      Author: talha
 */

#include <stdint.h>
#include <stdio.h>
#include "stm32h7sxx.h"
#include "stm32h7sxx_gpio_driver.h"
#include "stm32h7sxx_eth_driver.h"

/*
 * Forza UDP packet selection
 */
#define FORZA_PACKET_LEN        324U
#define FORZA_RPM_OFFSET        16U
#define FORZA_SPEED_OFFSET      256U

/*
 * Test UDP Port
 *
 * Forza Data Out Port should be set to this port.
 */
#define TEST_UDP_PORT           5005U

/*
 * Debug variables
 *
 * These variables are watched in debugger.
 */
volatile uint8_t  dbg_init_ok = 0U;
volatile uint8_t  dbg_phy_addr = 0U;
volatile uint8_t  dbg_frame_available = 0U;
volatile uint8_t  dbg_udp_available = 0U;

volatile uint32_t dbg_frame_addr = 0U;
volatile uint32_t dbg_frame_length = 0U;
volatile uint32_t dbg_ethertype = 0U;

volatile uint32_t dbg_payload_addr = 0U;
volatile uint32_t dbg_payload_length = 0U;

volatile uint8_t dbg_payload_0 = 0U;
volatile uint8_t dbg_payload_1 = 0U;
volatile uint8_t dbg_payload_2 = 0U;
volatile uint8_t dbg_payload_3 = 0U;
volatile uint8_t dbg_payload_4 = 0U;
volatile uint8_t dbg_payload_5 = 0U;
volatile uint8_t dbg_payload_6 = 0U;
volatile uint8_t dbg_payload_7 = 0U;
volatile uint8_t dbg_payload_8 = 0U;
volatile uint8_t dbg_payload_9 = 0U;
volatile uint8_t dbg_payload_10 = 0U;
volatile uint8_t dbg_payload_11 = 0U;
volatile uint8_t dbg_payload_12 = 0U;
volatile uint8_t dbg_payload_13 = 0U;
volatile uint8_t dbg_payload_14 = 0U;
volatile uint8_t dbg_payload_15 = 0U;

volatile uint8_t dbg_rpm_b0 = 0U;
volatile uint8_t dbg_rpm_b1 = 0U;
volatile uint8_t dbg_rpm_b2 = 0U;
volatile uint8_t dbg_rpm_b3 = 0U;

volatile uint8_t dbg_speed_b0 = 0U;
volatile uint8_t dbg_speed_b1 = 0U;
volatile uint8_t dbg_speed_b2 = 0U;
volatile uint8_t dbg_speed_b3 = 0U;

volatile uint32_t dbg_desc_index = 0U;
volatile uint32_t dbg_desc3 = 0U;

volatile uint32_t dbg_maccr = 0U;
volatile uint32_t dbg_macpfr = 0U;
volatile uint32_t dbg_mtlrxqomr = 0U;
volatile uint32_t dbg_dmacrxcr = 0U;

volatile uint16_t dbg_speed_kmh = 0U;
volatile uint16_t dbg_rpm = 0U;

volatile uint32_t dbg_forza_packet_count = 0U;
volatile uint32_t dbg_bad_len_count = 0U;

volatile uint32_t dbg_parse_stage = 0U;
volatile uint32_t dbg_rpm_raw = 0U;
volatile uint32_t dbg_speed_raw = 0U;

volatile float dbg_rpm_float = 0.0f;
volatile float dbg_speed_mps_float = 0.0f;
volatile float dbg_speed_kmh_float = 0.0f;

volatile uint32_t dbg_error_code = 0U;
volatile uint32_t dbg_init_step = 0U;

#define FORZA_IS_RACE_ON_OFFSET     0U
#define FORZA_TIMESTAMP_OFFSET      4U

volatile int32_t  dbg_is_race_on = 0;
volatile uint32_t dbg_timestamp_ms = 0U;

#define SCB_CPACR_ADDR          (0xE000ED88UL)
#define SCB_CPACR               (*(volatile uint32_t *)SCB_CPACR_ADDR)

static void FPU_Enable(void)
{
	/*
	 * Enable CP10 and CP11 full access.
	 * Required before using floating point instructions on Cortex-M7.
	 */
	SCB_CPACR |= (0xFU << 20U);

	/*
	 * Data and instruction synchronization barriers.
	 */
	__asm volatile ("dsb");
	__asm volatile ("isb");
}

/*
 * Private helper functions
 */
static uint32_t ReadU32LE(const uint8_t *data, uint32_t offset)
{
	return ((uint32_t)data[offset])              |
	       ((uint32_t)data[offset + 1U] << 8U)  |
	       ((uint32_t)data[offset + 2U] << 16U) |
	       ((uint32_t)data[offset + 3U] << 24U);
}

static float ReadF32LE(const uint8_t *data, uint32_t offset)
{
	union
	{
		uint32_t u32;
		float f32;
	} value;

	value.u32 = ReadU32LE(data, offset);

	return value.f32;
}

void Test_000_ETH_Forza_Test(void)
{
	FPU_Enable();
	uint8_t phy_addr = 0U;
	uint8_t phy_found = 0U;
	uint8_t eth_reset_ok = 0U;

	uint8_t mac[ETH_MAC_ADDR_LEN] = {
			0x02U, 0x00U, 0x00U, 0x00U, 0x00U, 0x01U
	};

	uint8_t *frame = 0;
	uint32_t frame_length = 0U;
	uint8_t frame_available = 0U;

	uint8_t *payload = 0;
	uint32_t payload_length = 0U;
	uint8_t udp_available = 0U;

	/*
	 * Config non-cacheable ETH-DMA Memory
	 */
	dbg_init_step = 1U;
	ETH_MPU_ConfigNonCacheable();

	/*
	 * Basic Ethernet Peripherals Init
	 */
	dbg_init_step = 2U;
	ETH_PeriClockControl(ENABLE);
	ETH_GPIO_RMII_Init();
	ETH_SelectRMII();

	/*
	 * Ethernet software reset
	 */
	dbg_init_step = 3U;
	eth_reset_ok = ETH_SoftwareReset();

	if(eth_reset_ok == 0U)
	{
		dbg_error_code = 1U;
		dbg_init_ok = 0U;
		while(1);
	}

	/*
	 * PHY Address scan
	 */
	dbg_init_step = 4U;
	phy_found = ETH_PHY_FindAddress(&phy_addr);

	if(phy_found == 0U)
	{
		dbg_error_code = 2U;
		dbg_init_ok = 0U;
		while(1);
	}

	dbg_phy_addr = phy_addr;

	/*
	 * Wait for link
	 */
	dbg_init_step = 5U;
	if(ETH_PHY_WaitForLink(phy_addr, ETH_TIMEOUT_VALUE) == 0U)
	{
		dbg_error_code = 3U;
		dbg_init_ok = 0U;
		while(1);
	}

	/*
	 * Wait for auto-negotiation
	 */
	dbg_init_step = 6U;
	if(ETH_PHY_WaitAutoNegotiation(phy_addr, ETH_TIMEOUT_VALUE) == 0U)
	{
		dbg_error_code = 4U;
		dbg_init_ok = 0U;
		while(1);
	}

	/*
	 * Set MAC Address
	 */
	dbg_init_step = 7U;
	ETH_SetMACAddress(mac);

	/*
	 * MAC Op mode config
	 *
	 * FES = 1 -> 100 Mbps
	 * DM  = 1 -> Full duplex
	 * RE  = 1 -> RX enable
	 * TE  = 1 -> TX enable
	 */
	dbg_init_step = 8U;
	ETH_MAC_OPModeConfig(
			ETH_MACCR_FES_MSK |
			ETH_MACCR_DM_MSK  |
			ETH_MACCR_RE_MSK  |
			ETH_MACCR_TE_MSK,

			ETH_MACCR_FES_MSK |
			ETH_MACCR_DM_MSK  |
			ETH_MACCR_RE_MSK  |
			ETH_MACCR_TE_MSK
	);

	/*
	 * MAC packet filtering
	 *
	 * PM = 1 -> Promiscuous mode
	 */
	dbg_init_step = 9U;
	ETH_MAC_FilterConfig(ENABLE, ETH_MACPFR_PM_MSK);

	/*
	 * Init RX Descriptors before starting DMA
	 */
	dbg_init_step = 10U;
	ETH_RXDesc_Init();

	/*
	 * MTL RX Queue Configuration
	 *
	 * RSF = 1 -> Receive store-and-forward mode
	 */
	dbg_init_step = 11U;
	ETH_MTL_RXQueueConfig(ENABLE, ETH_MTLRXQOMR_RSF_MSK);

	/*
	 * Configure and start RX DMA
	 */
	dbg_init_step = 12U;
	ETH_DMA_RXConfig();
	ETH_DMA_RXStart();

	dbg_init_step = 13U;
	dbg_error_code = 0U;
	dbg_init_ok = 1U;

	while(1)
	{
		frame = 0;
		frame_length = 0U;
		payload = 0;
		payload_length = 0U;
		udp_available = 0U;

		dbg_udp_available = 0U;
		dbg_payload_length = 0U;
		dbg_parse_stage = 0U;

		frame_available = ETH_ReadRawFrame(&frame, &frame_length);

		dbg_frame_available = frame_available;

		/*
		 * Debug registers
		 */
		dbg_desc_index = ETH_GetRXCurrentDescIndex();
		dbg_desc3 = ETH_GetRXDesc3(dbg_desc_index);

		dbg_maccr = ETH->MACCR;
		dbg_macpfr = ETH->MACPFR;
		dbg_mtlrxqomr = ETH->MTLRXQOMR;
		dbg_dmacrxcr = ETH->DMACRXCR;

		if(frame_available == 1U)
		{
			dbg_frame_addr = (uint32_t)frame;
			dbg_frame_length = frame_length;

			/*
			 * Read Ethertype
			 */
			dbg_ethertype = ETH_FrameTypeCheck(frame, frame_length);

			/*
			 * Extract UDP payload
			 */
			udp_available = ETH_GetUDPPayload(frame,
			                                  frame_length,
			                                  TEST_UDP_PORT,
			                                  &payload,
			                                  &payload_length);

			dbg_udp_available = udp_available;

			if(udp_available == 1U)
			{
				dbg_payload_addr = (uint32_t)payload;
				dbg_payload_length = payload_length;

				/*
				 * First 16 payload bytes
				 */
				if(payload_length >= 16U)
				{
					dbg_payload_0  = payload[0];
					dbg_payload_1  = payload[1];
					dbg_payload_2  = payload[2];
					dbg_payload_3  = payload[3];
					dbg_payload_4  = payload[4];
					dbg_payload_5  = payload[5];
					dbg_payload_6  = payload[6];
					dbg_payload_7  = payload[7];
					dbg_payload_8  = payload[8];
					dbg_payload_9  = payload[9];
					dbg_payload_10 = payload[10];
					dbg_payload_11 = payload[11];
					dbg_payload_12 = payload[12];
					dbg_payload_13 = payload[13];
					dbg_payload_14 = payload[14];
					dbg_payload_15 = payload[15];
				}

				/*
				 * Forza dash packet parse
				 */
				if(payload_length == FORZA_PACKET_LEN)
				{
					float rpm;
					float speed_mps;
					float speed_kmh;

					dbg_parse_stage = 1U;

					dbg_is_race_on = (int32_t)ReadU32LE(payload, FORZA_IS_RACE_ON_OFFSET);
					dbg_timestamp_ms = ReadU32LE(payload, FORZA_TIMESTAMP_OFFSET);

					dbg_rpm_b0 = payload[FORZA_RPM_OFFSET + 0U];
					dbg_rpm_b1 = payload[FORZA_RPM_OFFSET + 1U];
					dbg_rpm_b2 = payload[FORZA_RPM_OFFSET + 2U];
					dbg_rpm_b3 = payload[FORZA_RPM_OFFSET + 3U];

					dbg_speed_b0 = payload[FORZA_SPEED_OFFSET + 0U];
					dbg_speed_b1 = payload[FORZA_SPEED_OFFSET + 1U];
					dbg_speed_b2 = payload[FORZA_SPEED_OFFSET + 2U];
					dbg_speed_b3 = payload[FORZA_SPEED_OFFSET + 3U];

					dbg_rpm_raw = ReadU32LE(payload, FORZA_RPM_OFFSET);
					dbg_speed_raw = ReadU32LE(payload, FORZA_SPEED_OFFSET);

					dbg_parse_stage = 2U;

					rpm = ReadF32LE(payload, FORZA_RPM_OFFSET);
					speed_mps = ReadF32LE(payload, FORZA_SPEED_OFFSET);

					dbg_parse_stage = 3U;

					speed_kmh = speed_mps * 3.6f;

					dbg_rpm_float = rpm;
					dbg_speed_mps_float = speed_mps;
					dbg_speed_kmh_float = speed_kmh;

					if(rpm < 0.0f)
					{
						rpm = 0.0f;
					}

					if(speed_kmh < 0.0f)
					{
						speed_kmh = 0.0f;
					}

					dbg_parse_stage = 4U;

					dbg_rpm = (uint16_t)(rpm + 0.5f);
					dbg_speed_kmh = (uint16_t)(speed_kmh + 0.5f);

					dbg_parse_stage = 5U;

					dbg_forza_packet_count++;
				}
				else
				{
					dbg_bad_len_count++;
				}
			}

			ETH_ReleaseRXDescriptor();
		}
	}
}
