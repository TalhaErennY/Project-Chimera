/*
 * 004I2C_OLED_Print.c
 *
 *  Created on: 5 Haz 2026
 *      Author: talha
 */

#include <stdio.h>
#include <stdint.h>

#include "stm32h7sxx.h"
#include "stm32h7sxx_gpio_driver.h"
#include "stm32h7sxx_i2c_driver.h"
#include "ssd1306.h"

I2C_Handle_t I2C1Handle;

void I2C1_GPIOInits(void);
void I2C1_Inits(void);
static void delay(void);


void Test_004_I2C_ssd1306(void){
	uint8_t status;
	uint8_t throttle = 0U;

	Dashboard_Data_t dash;

	I2C1_GPIOInits();
	I2C1_Inits();

	status = SSD1306_Init(&I2C1Handle);

	if(status != I2C_OK){
		//Breakpoint here to check
		while(1);
	}

	//fake telemetry
	while(1){
		dash.throttle = throttle;
		dash.brakeFL = throttle;
		dash.brakeFR = 100U - throttle;
		dash.brakeRL = throttle / 2U;
		dash.brakeRR = (throttle > 50U) ? 100U : 20U;
		dash.speed = throttle * 3U;
		dash.timer_ms = throttle * 10U;

		//if the data is same wont update
		SSD1306_DrawDashboard(&dash);

		//if buffer changed, send it to OLED
		status = SSD1306_UpdateScreen();

		if(status != I2C_OK){
			while(1);
		}

		throttle++;

		if(throttle > 100U){
			throttle = 0U;
		}

		delay();
	}
	return;
}

//Functions************************************************************

/*
 * PINs
 * PB8 -> I2C1_SCL
 * PB9 -> I2C1_SDA
 */
void I2C1_GPIOInits(void){
	GPIO_Handle_t I2CPins;

	I2CPins.pGPIOx = GPIOB;

	I2CPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_AF;
	I2CPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
	I2CPins.GPIO_PinConfig.GPIO_PinPuPdCtrl = GPIO_PUPD_PU;
	I2CPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_HIGH;
	I2CPins.GPIO_PinConfig.GPIO_PinAltFuncMode = 4U;

	GPIO_PeriClockControl(GPIOB, ENABLE);

	I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_8;
	GPIO_Init(&I2CPins);

	I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_9;
	GPIO_Init(&I2CPins);
}

void I2C1_Inits(void){
	I2C1Handle.pI2Cx = I2C1;

	I2C1Handle.I2C_Config.I2C_Timing = I2C_TIMING_100KHZ;

	//stm32 master
	I2C1Handle.I2C_Config.I2C_DeviceAddress  = 0U;
	I2C1Handle.I2C_Config.I2C_NoStretchMode  = I2C_STRETCH_ENABLE;
	I2C1Handle.I2C_Config.I2C_AddressingMode = I2C_ADDRMODE_7BIT;

	I2C_Init(&I2C1Handle);
}

static void delay(void){
	volatile uint32_t i;

	for(i = 0U; i < 300000U; i++){

	}
}

