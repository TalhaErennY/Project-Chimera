## Project Chimera

Project Chimera is an ongoing STM32H7Sxx bare-metal driver and real-time telemetry dashboard project.

The first milestone is to build a custom low-level driver layer for the STM32H7Sxx microcontroller family. After the driver layer is stable, the project will evolve into a real-time Forza telemetry dashboard that receives telemetry data from a PC and displays warnings and driving information using the STM32 board.

## Current Progress

- STM32H7Sxx memory map definitions
- RCC GPIO clock control
- GPIO initialization and deinitialization
- GPIO input/output APIs
- BSRR-based GPIO pin write
- ODR-based GPIO pin toggle

## Planned Features

- EXTI interrupt support
- UART driver
- I2C driver
- SPI driver
- Ethernet communication
- Real-time dashboard and warning system
