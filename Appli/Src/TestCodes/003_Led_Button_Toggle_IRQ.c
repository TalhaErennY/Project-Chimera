/*
 * 003_Led_Button_Toggle_IRQ.c
 *
 *  Created on: May 24, 2026
 *      Author: talha
 */


#include "TestCodes.h"
#include "stm32h7sxx.h"
#include "stm32h7sxx_gpio_driver.h"

void Test_003_Led_Button_Toggle_IRQ(void){
	GPIO_Handle_t GPIOLed;
	GPIO_Handle_t GPIOButton;

	//Button Connected to PB11
	GPIOButton.pGPIOx = GPIOB;
	GPIOButton.GPIO_PinConfig.GPIO_PinNumber   = GPIO_PIN_NO_11;
	GPIOButton.GPIO_PinConfig.GPIO_PinMode     = GPIO_MODE_IT_RT;
	GPIOButton.GPIO_PinConfig.GPIO_PinPuPdCtrl = GPIO_PUPD_PD;


	//LED Connected to PB10
	GPIOLed.pGPIOx = GPIOB;
	GPIOLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_10;
	GPIOLed.GPIO_PinConfig.GPIO_PinMode	  = GPIO_MODE_OUT;
	GPIOLed.GPIO_PinConfig.GPIO_PinPuPdCtrl = GPIO_PUPD_NONE;
	GPIOLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;

	GPIO_PeriClockControl(GPIOB, ENABLE);

	GPIO_Init(&GPIOLed);
	GPIO_Init(&GPIOButton);

	//IRQ Configuration
	GPIO_IRQInterruptConfig(IRQ_NO_EXTI11, ENABLE);
	GPIO_IRQPriorityConfig(IRQ_NO_EXTI11, NVIC_IRQ_PRIO15);

}

void EXTI11_IRQHandler(void){
	GPIO_ToggleOutputPin(GPIOB, GPIO_PIN_NO_10);
	for(int i = 0; i < 1000000; i++);

	//Reset Pending Bit
	GPIO_IRQHandling(11);
}
