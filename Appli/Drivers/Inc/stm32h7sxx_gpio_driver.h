/*
 * stm32h7sxx_gpio_driver.h
 *
 *  Created on: May 21, 2026
 *      Author: talha
 */

#ifndef INC_STM32H7SXX_GPIO_DRIVER_H_
#define INC_STM32H7SXX_GPIO_DRIVER_H_

#include "stm32h7sxx.h"

/*
 * GPIO pin configuration structure
 *
 * Select configuration values from the @GPIO_xxx macro groups below.
 */
typedef struct{
	uint8_t GPIO_PinNumber;				/* Select from @GPIO_PIN_NUMBERS */
	uint8_t GPIO_PinMode;				/* Select from @GPIO_PIN_MODES */
	uint8_t GPIO_PinSpeed;				/* Select from @GPIO_OUTPUT_SPEEDS */
	uint8_t GPIO_PinPuPdCtrl;			/* Select from @GPIO_PUPD_CONFIGS */
	uint8_t GPIO_PinOPType;				/* Select from @GPIO_OUTPUT_TYPES */
	uint8_t GPIO_PinAltFuncMode;		/* Select alternate function number from device datasheet */
}GPIO_PinConfig_t;

/*
 * GPIO handle structure
 */
typedef struct{
	GPIO_RegDef_t *pGPIOx;				/* Base address of GPIO port */
	GPIO_PinConfig_t GPIO_PinConfig;	/* GPIO pin configuration settings */
}GPIO_Handle_t;

/*
 * @GPIO_PIN_NUMBERS
 * GPIO pin number selection macros
 */
#define GPIO_PIN_NO_0			0U
#define GPIO_PIN_NO_1			1U
#define GPIO_PIN_NO_2			2U
#define GPIO_PIN_NO_3			3U
#define GPIO_PIN_NO_4			4U
#define GPIO_PIN_NO_5			5U
#define GPIO_PIN_NO_6			6U
#define GPIO_PIN_NO_7			7U
#define GPIO_PIN_NO_8			8U
#define GPIO_PIN_NO_9			9U
#define GPIO_PIN_NO_10			10U
#define GPIO_PIN_NO_11			11U
#define GPIO_PIN_NO_12			12U
#define GPIO_PIN_NO_13			13U
#define GPIO_PIN_NO_14			14U
#define GPIO_PIN_NO_15			15U

/*
 * @GPIO_PIN_MODES
 * GPIO pin mode selection macros
 */
#define GPIO_MODE_IN			0U	/* Input mode */
#define	GPIO_MODE_OUT			1U	/* General purpose output mode */
#define GPIO_MODE_AF			2U	/* Alternate function mode */
#define GPIO_MODE_ANALOG		3U	/* Analog mode, reset state */
#define GPIO_MODE_IT_FT			4U	/* Interrupt mode, falling edge trigger */
#define GPIO_MODE_IT_RT			5U	/* Interrupt mode, rising edge trigger */
#define GPIO_MODE_IT_RFT		6U	/* Interrupt mode, falling and rising edge trigger */

/*
 * @GPIO_OUTPUT_TYPES
 * GPIO output type selection macros
 */
#define GPIO_OP_TYPE_PP			0U	/* Output push-pull, reset state */
#define GPIO_OP_TYPE_OD			1U	/* Output open-drain */

/*
 * @GPIO_OUTPUT_SPEEDS
 * GPIO output speed selection macros
 */
#define GPIO_OSPEED_LOW			0U	/* Low speed */
#define GPIO_OSPEED_MEDIUM		1U	/* Medium speed */
#define GPIO_OSPEED_HIGH		2U	/* High speed */
#define GPIO_OSPEED_VHIGH		3U	/* Very high speed */

/*
 * @GPIO_PUPD_CONFIGS
 * GPIO pull-up / pull-down selection macros
 */
#define GPIO_PUPD_NONE			0U 	/* No pull-up, no pull-down */
#define GPIO_PUPD_PU			1U	/* Pull-up */
#define GPIO_PUPD_PD			2U	/* Pull-down */


/******************************************************************************************************
 * 								APIs Supported by This Driver
 *       For more information about the APIs, check the function definitions in stm32h7sxx_gpio_driver.c
 ******************************************************************************************************/

/*
 * Peripheral Clock Setup
 */
void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t ENorDI);

/*
 * GPIO Init - DeInit
 */
void GPIO_Init(GPIO_Handle_t *pGPIOHandle);
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx);

/*
 * Data Read - Write
 */
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber);
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx);
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t Value);
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t Value);
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber);

/*
 * IRQ Configuration and ISR Handling
 */
void GPIO_IRQInterruptConfig(uint8_t IRQNumber, uint8_t ENorDI);
void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority);
void GPIO_IRQHandling(uint8_t PinNumber);


#endif /* INC_STM32H7SXX_GPIO_DRIVER_H_ */

